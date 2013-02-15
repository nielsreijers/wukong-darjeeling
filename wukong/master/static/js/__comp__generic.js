// vim: ts=4 sw=4
function Generic()
{
	Block.prototype.init.call(this);
	this.type='Generic';
	this.addSignal( new Signal('dummy'));
}


Generic.prototype = new Block();
Generic.prototype.constructor = Generic;
Block.register('Generic',Generic);