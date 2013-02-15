// vim: ts=4 sw=4
function Occupancy_Sensor()
{
	Block.prototype.init.call(this);
	this.type='Occupancy_Sensor';
	this.addAction( new Action('occupied'));
	this.addSignal( new Signal('occupied'));
}


Occupancy_Sensor.prototype = new Block();
Occupancy_Sensor.prototype.constructor = Occupancy_Sensor;
Block.register('Occupancy_Sensor',Occupancy_Sensor);