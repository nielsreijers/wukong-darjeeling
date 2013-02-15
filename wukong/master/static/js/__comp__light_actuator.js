// vim: ts=4 sw=4
function Light_Actuator()
{
	Block.prototype.init.call(this);
	this.type='Light_Actuator';
	this.addAction( new Action('on_off'));
	this.addSignal( new Signal('on_off'));
}


Light_Actuator.prototype = new Block();
Light_Actuator.prototype.constructor = Light_Actuator;
Block.register('Light_Actuator',Light_Actuator);