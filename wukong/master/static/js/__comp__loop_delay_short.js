// vim: ts=4 sw=4
function Loop_delay_short()
{
	Block.prototype.init.call(this);
	this.type='Loop_delay_short';
	this.addAction( new Action('input'));
	this.addAction( new Action('delay'));
	this.addSignal( new Signal('delay'));
	this.addSignal( new Signal('output'));
}


Loop_delay_short.prototype = new Block();
Loop_delay_short.prototype.constructor = Loop_delay_short;
Block.register('Loop_delay_short',Loop_delay_short);