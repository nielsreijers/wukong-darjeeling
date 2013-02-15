// vim: ts=4 sw=4
function Light_Sensor()
{
	Block.prototype.init.call(this);
	this.type='Light_Sensor';
	this.addSignal( new Signal('current_value'));
	this.addAction( new Action('refresh_rate'));
	this.addSignal( new Signal('refresh_rate'));
}


Light_Sensor.prototype = new Block();
Light_Sensor.prototype.constructor = Light_Sensor;
Block.register('Light_Sensor',Light_Sensor);