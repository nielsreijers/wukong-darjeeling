// vim: ts=4 sw=4
function Logical()
{
	Block.prototype.init.call(this);
	this.type='Logical';
	this.addAction( new Action('input1'));
	this.addAction( new Action('input2'));
	this.addAction( new Action('input3'));
	this.addAction( new Action('input4'));
	this.addAction( new Action('operator'));
	this.addSignal( new Signal('operator'));
	this.addSignal( new Signal('output'));
}


Logical.prototype = new Block();
Logical.prototype.constructor = Logical;
Block.register('Logical',Logical);