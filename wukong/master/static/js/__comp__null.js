// vim: ts=4 sw=4
function Null()
{
	Block.prototype.init.call(this);
	this.type='Null';
	this.addAction( new Action('null'));
	this.addSignal( new Signal('null'));
	this.addAction( new Action('refresh_rate'));
	this.addSignal( new Signal('refresh_rate'));
}


Null.prototype = new Block();
Null.prototype.constructor = Null;
Block.register('Null',Null);