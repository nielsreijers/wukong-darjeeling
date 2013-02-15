// vim: ts=4 sw=4
function Sound_Sensor()
{
	Block.prototype.init.call(this);
	this.type='Sound_Sensor';
	this.addSignal( new Signal('current_value'));
	this.addAction( new Action('refresh_rate'));
	this.addSignal( new Signal('refresh_rate'));
}


Sound_Sensor.prototype = new Block();
Sound_Sensor.prototype.constructor = Sound_Sensor;
Block.register('Sound_Sensor',Sound_Sensor);