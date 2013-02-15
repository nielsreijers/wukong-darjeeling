// vim: ts=4 sw=4
function PIR_Sensor()
{
	Block.prototype.init.call(this);
	this.type='PIR_Sensor';
	this.addSignal( new Signal('current_value'));
	this.addAction( new Action('refresh_rate'));
	this.addSignal( new Signal('refresh_rate'));
}


PIR_Sensor.prototype = new Block();
PIR_Sensor.prototype.constructor = PIR_Sensor;
Block.register('PIR_Sensor',PIR_Sensor);