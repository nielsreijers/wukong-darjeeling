#include <string.h>
#include "types.h"
#include "debug.h"
#include "wkreprog_impl.h"
#include <avr/boot.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// This function should be in the NRWW section to allow it to write to flash
//void BOOTLOADER_SECTION avr_flash_program_page(uint32_t page, uint8_t *buf);
void __attribute__ ((section (".reprogram_flash_page"))) avr_flash_program_page (uint32_t page, uint8_t *buf);
extern unsigned char di_app_archive_data[];

// TODO (if we run short on RAM): this can probably be made more
// efficient by splitting this up in two parts and writing directly
// to the flash page buffer while receiving the code using
// boot_page_fill, and then writing the buffer to flash in a separate
// function.
// Alternatively we could try to reclaim heap space from the vm first,
// since it can only reboot once we've started the reprogramming phase.
uint8_t avr_flash_pagebuffer[SPM_PAGESIZE];
uint32_t avr_flash_pageaddress = 0;
uint16_t avr_flash_buf_len = 0;

uint16_t wkreprog_impl_get_page_size() {
	return SPM_PAGESIZE;
}

void wkreprog_impl_open() {
	void *x = (void *)di_app_archive_data;
	avr_flash_pageaddress = (unsigned int)x;
	if (avr_flash_pageaddress % SPM_PAGESIZE != 0) {
		DEBUG_LOG(DBG_WKREPROG, "Flashing to address 0x%x: not a page boundary.", di_app_archive_data);
		avr_flash_pageaddress = 0;
	}
	DEBUG_LOG(DBG_WKREPROG, "Start writing to flash at address 0x%x.\n", di_app_archive_data);
	memset(avr_flash_pagebuffer, 0xFF, SPM_PAGESIZE); // Clear flash buffer
	avr_flash_buf_len = 0;
}

void wkreprog_impl_write(uint8_t size, uint8_t* data) {
	if (avr_flash_pageaddress == 0)
		return;
	DEBUG_LOG(DBG_WKREPROG, "Received %d bytes to flash to page 0x%x.\n", size, avr_flash_pageaddress);
	DEBUG_LOG(DBG_WKREPROG, "Buffer already contains %d bytes.\n", avr_flash_buf_len);
	while(size!=0) {
		uint8_t bytes_on_this_page = size;
		if (avr_flash_buf_len + size > SPM_PAGESIZE) {
			// Only 1 page at a time
			bytes_on_this_page = SPM_PAGESIZE-avr_flash_buf_len;
		}
		memcpy(avr_flash_pagebuffer + avr_flash_buf_len, data, bytes_on_this_page); // Copy the data to the page buffer
		if (avr_flash_buf_len + bytes_on_this_page == SPM_PAGESIZE) { // If we filled a whole page, write it to flash
			DEBUG_LOG(DBG_WKREPROG, "Flashing page at 0x%x.\n", avr_flash_pageaddress);
			avr_flash_program_page(avr_flash_pageaddress, avr_flash_pagebuffer);
			memset(avr_flash_pagebuffer, 0xFF, SPM_PAGESIZE); // Clear flash buffer
			avr_flash_pageaddress += SPM_PAGESIZE;
		}
		avr_flash_buf_len = (avr_flash_buf_len + bytes_on_this_page) % SPM_PAGESIZE;
		size -= bytes_on_this_page;
		data += bytes_on_this_page;
	}
}

void wkreprog_impl_close() {
	DEBUG_LOG(DBG_WKREPROG, "Closing flash file.\n");
	if (avr_flash_buf_len != 0) { // If there's any data remaining, write it to flash.
	  DEBUG_LOG(DBG_WKREPROG, "Flashing page at 0x%x.\n", avr_flash_pageaddress);
		avr_flash_program_page(avr_flash_pageaddress, avr_flash_pagebuffer);
	}
	avr_flash_pageaddress = 0;
}

void wkreprog_impl_reboot() {
	// Reset using the watchdog timer.
	WDTCSR = _BV(WDCE);
	WDTCSR = _BV(WDE);
	while (1) {
	}
}

// Copied from avr/boot.h example
void avr_flash_program_page (uint32_t page, uint8_t *buf)
{
	uint16_t i;
	uint8_t sreg;

	// Disable interrupts.
	sreg = SREG;
	cli();

	eeprom_busy_wait ();

	boot_page_erase (page);
	boot_spm_busy_wait ();      // Wait until the memory is erased.

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.
		uint16_t w = *buf++;
		w += (*buf++) << 8;
		boot_page_fill (page + i, w);
	}

	boot_page_write (page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.
	boot_rww_enable ();

	// Re-enable interrupts (if they were ever enabled).
	SREG = sreg;
}
