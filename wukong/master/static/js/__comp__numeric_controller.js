// vim: ts=4 sw=4
function Numeric_Controller()
{
	Block.prototype.init.call(this);
	this.type='Numeric_Controller';
	this.addAction( new Action('output'));
	this.addSignal( new Signal('output'));
}


Numeric_Controller.prototype = new Block();
Numeric_Controller.prototype.constructor = Numeric_Controller;
Block.register('Numeric_Controller',Numeric_Controller);