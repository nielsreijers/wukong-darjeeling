// vim: ts=4 sw=4
function If_boolean()
{
	Block.prototype.init.call(this);
	this.type='If_boolean';
	this.addAction( new Action('condition'));
	this.addAction( new Action('if_true'));
	this.addAction( new Action('if_false'));
	this.addSignal( new Signal('output'));
}


If_boolean.prototype = new Block();
If_boolean.prototype.constructor = If_boolean;
Block.register('If_boolean',If_boolean);