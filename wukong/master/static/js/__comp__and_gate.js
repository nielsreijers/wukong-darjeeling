// vim: ts=4 sw=4
function And_Gate()
{
	Block.prototype.init.call(this);
	this.type='And_Gate';
	this.addAction( new Action('input1'));
	this.addAction( new Action('input2'));
	this.addSignal( new Signal('output'));
}


And_Gate.prototype = new Block();
And_Gate.prototype.constructor = And_Gate;
Block.register('And_Gate',And_Gate);