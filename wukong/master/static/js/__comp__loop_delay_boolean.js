// vim: ts=4 sw=4
function Loop_delay_boolean()
{
	Block.prototype.init.call(this);
	this.type='Loop_delay_boolean';
	this.addAction( new Action('input'));
	this.addAction( new Action('delay'));
	this.addSignal( new Signal('delay'));
	this.addSignal( new Signal('output'));
}


Loop_delay_boolean.prototype = new Block();
Loop_delay_boolean.prototype.constructor = Loop_delay_boolean;
Block.register('Loop_delay_boolean',Loop_delay_boolean);