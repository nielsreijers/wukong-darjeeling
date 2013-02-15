// vim: ts=4 sw=4
function If_short()
{
	Block.prototype.init.call(this);
	this.type='If_short';
	this.addAction( new Action('condition'));
	this.addAction( new Action('if_true'));
	this.addAction( new Action('if_false'));
	this.addSignal( new Signal('output'));
}


If_short.prototype = new Block();
If_short.prototype.constructor = If_short;
Block.register('If_short',If_short);