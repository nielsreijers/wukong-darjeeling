// vim: ts=4 sw=4
function Threshold()
{
	Block.prototype.init.call(this);
	this.type='Threshold';
	this.addAction( new Action('operator'));
	this.addSignal( new Signal('operator'));
	this.addAction( new Action('threshold'));
	this.addSignal( new Signal('threshold'));
	this.addAction( new Action('value'));
	this.addSignal( new Signal('value'));
	this.addSignal( new Signal('output'));
}


Threshold.prototype = new Block();
Threshold.prototype.constructor = Threshold;
Block.register('Threshold',Threshold);