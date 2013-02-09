#include "config.h" // To get RADIO_USE_ZWAVE

#ifdef RADIO_USE_ZWAVE

#include "types.h"
#include "panic.h"
#include "debug.h"
#include "djtimer.h"
#include "uart.h"
#include "wkcomm.h"

#define WKCOMM_PANIC_INIT_FAILED 100 // Need to make sure these codes don't overlap with other libs or the definitions in panic.h

address_t wkcomm_zwave_my_address;

void nvmcomm_zwave_init(void); // Refactor this later
void wkcomm_zwave_init(void) {
	nvmcomm_zwave_init();
}

address_t wkcomm_zwave_get_node_id() {
	return wkcomm_zwave_my_address;
}

void wkcomm_zwave_poll(void) {

}

uint8_t wkcomm_zwave_send(address_t dest, uint8_t command, uint8_t *payload, uint8_t length, uint16_t seqnr) {
	dj_panic(DJ_PANIC_UNIMPLEMENTED_FEATURE);
	return 1; // To keep the compiler happy.
}







///// temporary?
void delay(uint32_t msec) {
	dj_time_t start = dj_timer_getTimeMillis();
	while (dj_timer_getTimeMillis() < start+msec) {
		// NOOP
	}
}

///// Below is the original code from nvmcomm_zwave.c

#define ZWAVE_UART              	 2
#define ZWAVE_UART_BAUDRATE			 115200

#define ZWAVE_STATUS_WAIT_ACK        0
#define ZWAVE_STATUS_WAIT_SOF        1
#define ZWAVE_STATUS_WAIT_LEN        2
#define ZWAVE_STATUS_WAIT_TYPE       3
#define ZWAVE_STATUS_WAIT_CMD        4
#define ZWAVE_STATUS_WAIT_ID         5
#define ZWAVE_STATUS_WAIT_DATA       6
#define ZWAVE_STATUS_WAIT_CRC        7
#define ZWAVE_STATUS_WAIT_DONE       8

#define ZWAVE_TYPE_REQ          0x00
#define ZWAVE_TYPE_CMD          0x01

#define ZWAVE_CMD_APPLICATIONCOMMANDHANDLER 0x04

#define COMMAND_CLASS_PROPRIETARY   0x88

#define ZWAVE_REQ_SENDDATA     0x13
#define FUNC_ID_MEMORY_GET_ID  0x20

#define ZWAVE_ACK              0x06

// u16_t g_seq = 0;
uint8_t state;        // Current state
uint8_t len;          // Length of the returned payload
uint8_t type;         // 0: request 1: response 2: timeout
uint8_t cmd;          // the serial api command number of the current payload
uint8_t payload[WKCOMM_MESSAGE_SIZE+5];  // The data of the current packet. 
// 4 bytes protocol overhead (see nvmcomm_zwave_receive),
// 1 byte for the nvc3_command, which is the first byte in the buffer.
uint8_t payload_length;  // Length of the payload while reading a packet
// TODO: used?
uint8_t last_node = 0;
uint8_t seq;          // Sequence number which is used to match the callback function
uint8_t ack_got = 0;
int zwsend_ack_got = 0;
uint8_t wait_CAN_NAK = 1;
uint8_t zwave_learn_on = 0;
uint8_t zwave_learn_block = 0;
uint32_t zwave_learn_startT;
uint8_t zwave_learn_mode;
// uint32_t expire;  // The expire time of the last command
uint32_t nvmcomm_zwaveLastByteTime;

bool wkcomm_zwave_my_address_loaded = false;

void (*f)(address_t src, uint8_t nvc3_command, uint8_t *payload, uint8_t length); // The callback function registered by callback
void (*f_nodeinfo)(uint8_t *payload, uint8_t length);

int ZW_GetRoutingInformation(uint8_t id);
int ZW_sendData(uint8_t id, uint8_t nvc3_command, uint8_t *in, uint8_t len, uint8_t txoptions);
int SerialAPI_request(unsigned char *buf, int len);

bool addr_nvmcomm_to_zwave(address_t nvmcomm_addr, uint8_t *zwave_addr) {
    // Temporary: addresses <128 are ZWave, addresses >=128 are XBee
    if (nvmcomm_addr>=128)
        return false;
    *zwave_addr = nvmcomm_addr;
    return true;
}

bool addr_zwave_to_nvmcomm(address_t *nvmcomm_addr, uint8_t zwave_addr) {
    if (zwave_addr>=128)
        return false;
    *nvmcomm_addr = zwave_addr;
    return true;
}


// Blocking receive.
// Returns the Z-Wave cmd of the received message.
// Calls the callback for .... messages?
void nvmcomm_zwave_receive(int processmessages) {
    //DEBUG_LOG(DBG_ZWAVETRACE, "zwave receive!!!!!!!!!!!");
    while (!uart_available(ZWAVE_UART)) { }
    while (uart_available(ZWAVE_UART)) {
        // TODO    expire = now + 1000;
        uint8_t c = uart_read_byte(ZWAVE_UART);
        DEBUG_LOG(DBG_ZWAVETRACE, "c="DBG8" state="DBG8"\n\r", c, state);
        if (state == ZWAVE_STATUS_WAIT_ACK) {
            if (c == ZWAVE_ACK) {
                state = ZWAVE_STATUS_WAIT_SOF;
                wait_CAN_NAK = 1;
                ack_got=1;
            } else if (c == 0x15) {
                // send: no ACK from other side
                if (wait_CAN_NAK != 128)
                    wait_CAN_NAK *= 2;
                DEBUG_LOG(DBG_WKCOMM, "[NAK] SerialAPI LRC checksum error!!! delay: %dms\n", wait_CAN_NAK);
                delay(wait_CAN_NAK);
                state = ZWAVE_STATUS_WAIT_SOF;
                ack_got=0;
            } else if (c == 0x18) {
                // send: chip busy
                if (wait_CAN_NAK != 128)
                    wait_CAN_NAK *= 2;
                DEBUG_LOG(DBG_WKCOMM, "[CAN] SerialAPI frame is dropped by ZW!!! delay: %dms\n", wait_CAN_NAK);
                delay(wait_CAN_NAK);
                state = ZWAVE_STATUS_WAIT_SOF;
                ack_got=0;
            } else if (c == 1) {
                state = ZWAVE_STATUS_WAIT_LEN;
                len = 0;  
            } else {
                DEBUG_LOG(DBG_WKCOMM, "Unexpected byte while waiting for ACK %x\n", c);
            }
        } else if (state == ZWAVE_STATUS_WAIT_SOF) {
            if (c == 0x01) {
                state = ZWAVE_STATUS_WAIT_LEN;
                len = 0;
            } else if (c == 0x18) {
                DEBUG_LOG(DBG_WKCOMM, "ZWAVE_STATUS_WAIT_SOF: SerialAPI got CAN, we should wait for ACK\n");
                state = ZWAVE_STATUS_WAIT_ACK;
            } else if (c == ZWAVE_ACK) {
                DEBUG_LOG(DBG_WKCOMM, "ZWAVE_STATUS_WAIT_SOF: SerialAPI got unknown ACK ????????\n");
                ack_got = 1;
            }
        } else if (state == ZWAVE_STATUS_WAIT_LEN) {
            len = c-3; // 3 bytes for TYPE, CMD, and CRC
            state = ZWAVE_STATUS_WAIT_TYPE;
        } else if (state == ZWAVE_STATUS_WAIT_TYPE) {
            type = c; // 0: request 1: response 2: timeout
            state = ZWAVE_STATUS_WAIT_CMD;
        } else if (state == ZWAVE_STATUS_WAIT_CMD) {
            cmd = c;
            state = ZWAVE_STATUS_WAIT_DATA;
            payload_length = 0;
        } else if (state == ZWAVE_STATUS_WAIT_DATA) {
            payload[payload_length++] = c;
            len--;
            if (len == 0) {
                state = ZWAVE_STATUS_WAIT_CRC;
            }
        } else if (state == ZWAVE_STATUS_WAIT_CRC) {
            uart_write_byte(ZWAVE_UART, 6);
            state = ZWAVE_STATUS_WAIT_SOF;
            if (type == ZWAVE_TYPE_REQ && cmd == 0x13) {
                zwsend_ack_got = payload[1];
            }
            if (type == ZWAVE_TYPE_REQ && cmd == ZWAVE_CMD_APPLICATIONCOMMANDHANDLER)
                if (f!=NULL) {
                    address_t nvmcomm_addr;
                    if (addr_zwave_to_nvmcomm(&nvmcomm_addr, payload[1]) && processmessages==1)
                        f(nvmcomm_addr, payload[4], payload+5, payload_length-5); // Trim off first 5 bytes to get to the data. Byte 1 is the sending node, byte 4 is the command
                }
            if (cmd == FUNC_ID_MEMORY_GET_ID) {
                wkcomm_zwave_my_address = payload[4];
                wkcomm_zwave_my_address_loaded = true;
            }
            if (cmd == 0x49 && f_nodeinfo)
                f_nodeinfo(payload, payload_length);
            if (cmd == 0x50) {
                if(payload[1]==0x01) {
                    zwave_learn_block = 1;
                    //	   DEBUG_LOG(DBG_WKCOMM, "zwave payload block !!!!!!!!!!!!!!!!");
                }
                else if(payload[1]==6) {//network stop, learn off
                    unsigned char b[10];
                    unsigned char onoff=0;
                    int k;
                    b[0] = 1;
                    b[1] = 5;
                    b[2] = 0;
                    b[3] = 0x50;
                    b[4] = onoff;//off
                    b[5] = seq;
                    b[6] = 0xff^5^0^0x50^onoff^seq;
                    seq++;
                    //DEBUG_LOG(DBG_WKCOMM, "zwave payload learnoff !!!!!!!!!!!!!!!!");
                    for(k=0;k<7;k++)
                    {
                        //Serial1.write(b[k]);
                        uart_write_byte(ZWAVE_UART, b[k]);
                    }
                    zwave_learn_on=0;
                    zwave_learn_block=0;
                    zwave_learn_mode=0;
                }
            }
        }
    }
}



// Public interface
void nvmcomm_zwave_init() {

    // Clear existing queue on Zwave
    DEBUG_LOG(DBG_WKCOMM, "Clearing leftovers\n");
    while (uart_available(ZWAVE_UART)) {
        uart_read_byte(ZWAVE_UART);
    }

    // TODO: why is this here?
    // for(i=0;i<100;i++)
    //   mainloop();
    // TODO: analog read
    // randomSeed(analogRead(0));
    // seq = random(255);
    seq = 42; // temporarily init to fixed value
    state = ZWAVE_STATUS_WAIT_SOF;
    nvmcomm_zwaveLastByteTime = dj_timer_getTimeMillis();
    f=NULL;
    f_nodeinfo=NULL;
    uart_init(ZWAVE_UART, ZWAVE_UART_BAUDRATE);
    // TODO
    // expire = 0;

    unsigned char buf[] = {ZWAVE_TYPE_REQ, FUNC_ID_MEMORY_GET_ID};
    wkcomm_poll();
    uint8_t retries = 10;
    address_t previous_received_address = 0;

    DEBUG_LOG(DBG_WKCOMM, "Getting zwave address...\n");
    while(!wkcomm_zwave_my_address_loaded) {
        while(!wkcomm_zwave_my_address_loaded && retries-->0) {
            SerialAPI_request(buf, 2);
            wkcomm_poll();
        }
        if(!wkcomm_zwave_my_address_loaded) // Can't read address -> panic
            dj_panic(WKCOMM_PANIC_INIT_FAILED);
        if (wkcomm_zwave_my_address != previous_received_address) { // Sometimes I get the wrong address. Only accept if we get the same address twice in a row. No idea if this helps though, since I don't know what's going on exactly.
            wkcomm_zwave_my_address_loaded = false;
            previous_received_address = wkcomm_zwave_my_address;
        }
    }
    DEBUG_LOG(DBG_WKCOMM, "My Zwave node_id: %x\n", wkcomm_zwave_my_address);
}

void nvmcomm_zwave_setcallback(void (*func)(address_t, uint8_t, uint8_t *, uint8_t)) {
    f = func;
}

void nvmcomm_zwave_poll(void) {
    // TODO
    // unsigned long now = millis();
    // 
    // if (expire && (now > expire)) {
    //   expire = 0;
    //   type = 2;
    //   state = ZWAVE_STATUS_WAIT_SOF;
    //   if (f!=NULL) f(payload,i);
    //   Serial.write("timeout...\n");
    //   return true;
    // }
    if (uart_available(ZWAVE_UART))
    {    
        DEBUG_LOG(DBG_ZWAVETRACE, "data_available\n");
        nvmcomm_zwave_receive(1);
        /*nvmcomm_zwaveLastByteTime = dj_timer_getTimeMillis();*/
    } else {
        /*DEBUG_LOG(DBG_ZWAVETRACE, "data_not_available\n");*/
        // This will confuse Zwave state and make it stop running, so don't use it
        /*if (dj_timer_getTimeMillis() > nvmcomm_zwaveLastByteTime + 600) {*/
            /*state = ZWAVE_STATUS_WAIT_SOF;*/
            /*nvmcomm_zwaveLastByteTime = dj_timer_getTimeMillis();*/
            /*nvmcomm_zwave_receive(0);*/
        /*}*/
    }
}

// Send ZWave command to another node. This command can be used as wireless repeater between 
// two nodes. It has no assumption of the payload sent between them.
int nvmcomm_zwave_send(address_t dest, uint8_t nvc3_command, uint8_t *data, uint8_t len, uint8_t txoptions) {
#ifdef DEBUG
    DEBUG_LOG(DBG_WKCOMM, "Sending command "DBG8" to "DBG8", length "DBG8": ", nvc3_command, dest, len);
    for (size8_t i=0; i<len; ++i) {
        DEBUG_LOG(DBG_WKCOMM, " "DBG8"", data[i]);
    }
    DEBUG_LOG(DBG_WKCOMM, "\n");
#endif
    uint8_t zwave_addr;
    if (addr_nvmcomm_to_zwave(dest, &zwave_addr))
        return ZW_sendData(zwave_addr, nvc3_command, data, len, txoptions);
    else
        return -1; // Not a ZWave address
    // TODO  expire = millis()+1000;
}

// Get the ID of this node
address_t nvmcomm_zwave_get_node_id() {
    return wkcomm_zwave_my_address;
}


void nvmcomm_zwave_learn() {
    unsigned char b[10];
    unsigned char onoff=1;
    int k;    
    if(zwave_learn_on==0)
    {
        zwave_learn_startT=dj_timer_getTimeMillis();
        zwave_learn_on=1;
        b[0] = 1;
        b[1] = 5;
        b[2] = 0;
        b[3] = 0x50;
        b[4] = onoff;
        b[5] = seq;
        b[6] = 0xff^5^0^0x50^onoff^seq;
        seq++;
        //DEBUG_LOG(DBG_WKCOMM, "zwave learn !!!!!!!!!!!!!!!!");
        for(k=0;k<7;k++)
        {
            uart_write_byte(ZWAVE_UART, b[k]);
        }
    }
    //DEBUG_LOG(DBG_WKCOMM, "current:"DBG32" start:"DBG32", zwave_learn_block:"DBG8": ", dj_timer_getTimeMillis(), zwave_learn_startT, zwave_learn_block);
    if(dj_timer_getTimeMillis()-zwave_learn_startT>10000 && !zwave_learn_block) { //time out learn off
        // DEBUG_LOG(DBG_WKCOMM, "turn off!!!!!!!!!!!!!!!!");
        onoff=0;
        b[0] = 1;
        b[1] = 5;
        b[2] = 0;
        b[3] = 0x50;
        b[4] = onoff;//off
        b[5] = seq;
        b[6] = 0xff^5^0^0x50^onoff^seq;
        seq++;	   
        for(k=0;k<7;k++)
        {
            uart_write_byte(ZWAVE_UART, b[k]);
        }  
        zwave_learn_on=0;
        zwave_learn_block=0;
        zwave_learn_mode=0;
    }
}





//===================================================================================================================
// Copied & modified from testrtt.c
//===================================================================================================================
int SerialAPI_request(unsigned char *buf, int len)
{
    unsigned char c = 1;
    int i;
    unsigned char crc;
    int retry = 5;

    while (1) {
        // read out pending request from Z-Wave
        if (uart_available(ZWAVE_UART))
            nvmcomm_zwave_receive(0); // Don't process received messages
        if (state != ZWAVE_STATUS_WAIT_SOF) {	// wait for WAIT_SOF state (idle state)
            DEBUG_LOG(DBG_WKCOMM, "SerialAPI is not in ready state!!!!!!!!!! zstate="DBG8"\n", state);
            DEBUG_LOG(DBG_WKCOMM, "Try to send SerialAPI command in a wrong state......\n");
            delay(100);
            //continue;
        }

        // send SerialAPI request
        c=1;
        uart_write_byte(ZWAVE_UART, c); // SOF (start of frame)
        c = len+1;
        uart_write_byte(ZWAVE_UART, c); // len (length of frame)
        crc = 0xff;
        crc = crc ^ (len+1);
        for(i=0;i<len;i++) {
            crc = crc ^ buf[i];
            uart_write_byte(ZWAVE_UART, buf[i]); // REQ, cmd, data
        }
        uart_write_byte(ZWAVE_UART, crc); // LRC checksum
#ifdef DEBUG
        DEBUG_LOG(DBG_WKCOMM, "Send len="DBG8" ", len+1);
        for(i=0;i<len;i++) {
            DEBUG_LOG(DBG_WKCOMM, ""DBG8" ", buf[i]);
        }
        DEBUG_LOG(DBG_WKCOMM, "CRC="DBG8"\n", crc);
        ;
#endif
        state = ZWAVE_STATUS_WAIT_ACK;
        ack_got = 0;

        // get SerialAPI ack
        i = 0;
        while(!uart_available(ZWAVE_UART) && i++<100)
            delay(1);
        if (uart_available(ZWAVE_UART)) {
            nvmcomm_zwave_poll();			
            if (ack_got == 1) {
                return 0;
            } else {
                DEBUG_LOG(DBG_WKCOMM, "Ack error!!! zstate="DBG8" ack_got="DBG8"\n", state, ack_got);
            }
        } 
        if (state == ZWAVE_STATUS_WAIT_ACK) {
            state = ZWAVE_STATUS_WAIT_SOF; // Give up and don't get stuck in the WAIT_ACK state
            DEBUG_LOG(DBG_WKCOMM, "Back to WAIT_SOF state.\n");
        }
        if (!retry--) {
            DEBUG_LOG(DBG_WKCOMM, "SerialAPI request:\n");
            for (i=0; i<len; i++) {
                DEBUG_LOG(DBG_WKCOMM, ""DBG8" ", buf[i]);
            }
            DEBUG_LOG(DBG_WKCOMM, "\n");
            DEBUG_LOG(DBG_WKCOMM, "error!!!\n", __FUNCTION__);
            return -1;
        }
        DEBUG_LOG(DBG_WKCOMM, "SerialAPI_request retry (%d)......\n", retry);
    }
    return -1; // Never happens
}

/*
int ZW_GetRoutingInformation(uint8_t id)
{
    unsigned char buf[255];

    buf[0] = ZW_REQ;
    buf[1] = GetRoutingInformation;
    buf[2] = id;
    if (SerialAPI_request(buf, 3) != 0)
      return -1;
}
*/

int ZW_sendData(uint8_t id, uint8_t nvc3_command, uint8_t *in, uint8_t len, uint8_t txoptions)
{
    unsigned char buf[WKCOMM_MESSAGE_SIZE+8];
    int i;
    int timeout = 1000;
    zwsend_ack_got = -1;

    buf[0] = ZWAVE_TYPE_REQ;
    buf[1] = ZWAVE_REQ_SENDDATA;
    buf[2] = id;
    buf[3] = len+2;
    buf[4] = COMMAND_CLASS_PROPRIETARY;
    buf[5] = nvc3_command; // See nvmcomm.h
    for(i=0; i<len; i++)
        buf[i+6] = in[i];
    buf[6+len] = txoptions;
    buf[7+len] = seq++;
    if (SerialAPI_request(buf, len + 8) != 0)
        return -1;
    while (zwsend_ack_got == -1 && timeout-->0) {
        wkcomm_poll();
        delay(1);
    }
    if (zwsend_ack_got == 0) // ACK 0 indicates success
        return 0;
    else {
        DEBUG_LOG(DBG_WKCOMM, "========================================ZW_sendDATA ack got: %x\n", zwsend_ack_got);
        return -1;    
    }
}
//===================================================================================================================
// End: copied & modified from testrtt.c
//===================================================================================================================


// 
// public:
//   int getType() {
//     return type;
//   }
//   void DisplayNodeInfo() {
//     char buf[128];
//     
//     snprintf(buf,64,"Status=%d Node=%d Device=%d:%d:%d\n", payload[0],payload[1],payload[3],payload[4],payload[5]);
//     Serial.write(buf);
//   }
//   
// 
// 
//   
//   // Include or exclude node from the network
//   void networkIncludeExclude(byte t,byte m) {
//     byte b[10];
//     int k;
//   
//     b[0] = 1;
//     b[1] = 5;
//     b[2] = 0;
//     b[3] = t;
//     b[4] = m;
//     b[5] = seq;
//     b[6] = 0xff^5^0^t^m^seq;
//     seq++;
//     for(k=0;k<7;k++)
//       Serial1.write(b[k]);
//   }
//   
//   // Reset the ZWave module to the factory default value. This must be called carefully since it will make
//   // the network unusable.
//   void reset() {
//     byte b[10];
//     int k;
//   
//     b[0] = 1;
//     b[1] = 4;
//     b[2] = 0;
//     b[3] = 0x42;
//     b[4] = seq;
//     b[5] = 0xff^4^0^0x42^seq;
//     seq++;
//     for(k=0;k<6;k++)
//       Serial1.write(b[k]);
//         
//   }
//   // Start inclusion procedure to add a new node
//   void includeAny() {networkIncludeExclude(0x4A,1);}
// 
//   // Stop inclusion/exclusion procedure
//   void LearnStop() {networkIncludeExclude(0x4A,5);}
// 
//   // Start exclusion procedure
//   void excludeAny() {networkIncludeExclude(0x4B,1);}
// 
//   // Set the value of a node
//   void set(byte id,byte v,byte option) {
//     byte b[3];
//     
//     b[0] = 0x20;
//     b[1] = 1;
//     b[2] = v;
//     send(id,b,3,option);
//   }
// };
// 
// ZWaveClass ZWave;
// 
// void offack(byte *b,int len)
// {
//   if (ZWave.getType() == 0) {
//     delay(500);
//     ZWave.callback(onack);
//     ZWave.set(last_node,255,5);
//   } else if (ZWave.getType() == 2) {
//       Serial.write("Timeout\n");
//       ZWave.callback(onack);
//       ZWave.set(last_node,255,5);
//   } 
// }
// 
// void onack(byte *b,int len)
// {
//   if (ZWave.getType() == 0) {
//     delay(500);
//     ZWave.callback(offack);
//     ZWave.set(last_node,0,5);
//   } else if (ZWave.getType() == 2) {
//       Serial.write("Timeout\n");
//       ZWave.callback(offack);
//       ZWave.set(last_node,0,5);
//   }
// }
// 
// void include_cb(byte *b,int len)
// {
//   char buf[64];
//   
//   snprintf(buf,64,"Status=%d Node=%d\n", b[0],b[1]);
// }
// 
// void exclude_cb(byte *b,int len)
// {
//   char buf[64];
//   
//   snprintf(buf,64,"Status=%d Node=%d\n", b[0],b[1]);
// }
// 
// void help()
// {
//   Serial.write("a: Include a new node\n");
//   Serial.write("d: Exclude a node\n");
//   Serial.write("s: stop inclusion/exclusion\n");
//   Serial.write("t: test the current node\n");
//   Serial.write("Press the program button of the node to change the current node\n");
// }
//   
// void loop()
// {
//   if (ZWave.mainloop()) {
//     
//   }
//   if (Serial.available()) {
//     byte c = Serial.read();
//     if (c == 'a') {
//       ZWave.callback(include_cb);
//       ZWave.includeAny();
//     } else if (c == 'd') {
//       ZWave.callback(exclude_cb);
//       ZWave.excludeAny();
//     } else if (c == 's') {
//       ZWave.callback(0);
//       ZWave.LearnStop();
//     } else if (c == 't') {
//       ZWave.callback(offack);
//       ZWave.set(last_node,0,5);
//     } else {
//       help();
//     }
//   }
// }
//   





#endif


