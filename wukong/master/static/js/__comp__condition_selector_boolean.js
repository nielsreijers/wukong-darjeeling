// vim: ts=4 sw=4
function Condition_selector_boolean()
{
	Block.prototype.init.call(this);
	this.type='Condition_selector_boolean';
	this.addAction( new Action('input'));
	this.addAction( new Action('control'));
	this.addSignal( new Signal('control'));
	this.addSignal( new Signal('output1'));
	this.addSignal( new Signal('output2'));
}


Condition_selector_boolean.prototype = new Block();
Condition_selector_boolean.prototype.constructor = Condition_selector_boolean;
Block.register('Condition_selector_boolean',Condition_selector_boolean);