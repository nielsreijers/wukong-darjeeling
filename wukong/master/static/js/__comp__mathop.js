// vim: ts=4 sw=4
function MathOp()
{
	Block.prototype.init.call(this);
	this.type='MathOp';
	this.addAction( new Action('input1'));
	this.addAction( new Action('input2'));
	this.addAction( new Action('input3'));
	this.addAction( new Action('input4'));
	this.addAction( new Action('operator'));
	this.addSignal( new Signal('operator'));
	this.addSignal( new Signal('output'));
	this.addSignal( new Signal('remainder'));
}


MathOp.prototype = new Block();
MathOp.prototype.constructor = MathOp;
Block.register('MathOp',MathOp);