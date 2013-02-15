// vim: ts=4 sw=4
function Temperature_Sensor()
{
	Block.prototype.init.call(this);
	this.type='Temperature_Sensor';
	this.addSignal( new Signal('current_temperature'));
}


Temperature_Sensor.prototype = new Block();
Temperature_Sensor.prototype.constructor = Temperature_Sensor;
Block.register('Temperature_Sensor',Temperature_Sensor);