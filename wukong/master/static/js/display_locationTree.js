var landmark = {};

$("#addNode").click(function () {
	var add_node = $('#node_addDel').val(),
	    add_loc = $('#locName').val();
	landmark[add_node] = add_loc;
	console.log(landmark);
	$.ajax('/loc_tree/land_mark', {
        type: 'PUT',
        dataType: 'json',
        data: {ope: 1, name: add_node, location: $('#locName').val(),size:$('#size').val(), direction:$('#direction').val()},
        success: function(data) {
            //data = JSON.parse(data);
            //display update
             $.post('/loc_tree', function(data) {
	    		display_tree(data);
	    		$('#content').append(data.node);
	    		load_landmark(data.xml);
			});
            if (data.status === 1) {
                alert(data.mesg);
            }
        }
    });
});

$("#delNode").click(function(){
	delNode = $('#node_addDel').val();
	$.ajax('/loc_tree/land_mark', {
        type: 'PUT',
        dataType: 'json',
        data: {ope: 0, name: delnode},
        success: function(data) {
            //data = JSON.parse(data);
            //display update
             $.post('/loc_tree', function(data) {
	    		display_tree(data);
	    		delete landmark[delNode];
            	$("li").remove("#"+delNode);
			});
            if (data.status == 1) {
                alert(data.mesg);
            }
        }
    });
});
/*
$("#delNode").click(function(){
	var del_node = $('#node_addDel').val();
	$.ajax('/loc_tree/land_mark', {
        type: 'PUT',
        dataType: 'json',
        data: {ope: 0, name: del_node, location: $('#locName').val()},
        success: function(data) {
             $.post('/loc_tree', function(data) {
	    		display_tree(data);
	    		$('#content').append(data.node);
//	    		load_landmark(data.xml);
			});
            if (data.status == 1) {
                alert(data.mesg);
            }
        }
    });
});
*/
$("#saveTree").click(function(){
	save_landmark();
});
$("#addModifier").click(function(){
    add_modifier();
    $('#'+document.body.dataset.locTreeNodeId).dblclick();
});
$("#delModifier").click(function(){
    del_modifier();
    $('#'+document.body.dataset.locTreeNodeId).dblclick();
});
$('.btn').click(function(){
    dialog_content = '<button id="addNode">Add/Del Landmark</button>'+
     //'<button id="delNode">DEL Landmark</button>'+'<button id="addNode">ADD Landmark</button>'+'<button type="button" class="set_node">Save Node Configuration</button><br>'+
     'Distance Modifier <button id="addModifier">Add Modifier</button> '+ 
      '<button id="delModifier">Delete Modifier</button><br>' +
      'Existing Modifiers <div id="distmod_list"></div><br>' +
      'start treenode ID<input id = "distmod_start_id" type=text size="20"><br>'+
       'end treenode ID<input id = "distmod_end_id" type=text size="20"><br>'+
       'distance<input id = "distmod_distance" type=text size="20"><br>';
    var nodeId = $(this).attr("id");
    var nodeName = $(this).text();
    var locTreeNodeId = $(this).parent().parent().children('.locTreeNode').attr("id");
	$('#node_addDel').val(nodeId);
	$.get('/loc_tree/nodes/'+locTreeNodeId+'/'+nodeId, function(data) {
        if (data.status == '1') {
            alert(data.message);
        }else if (data.status=='0'){
            $('#locName').val(data.location);
            $('#SensorId').val(nodeId);
            if (nodeId[0]=='l'){
                $('#nodeType').html("landmark");
            }else{
                $('#nodeType').html("sensor");
            }
            $('#distmod_list').val("NA");
        }else{
            alert("received JASON format is wrong!!!");
        }
    });
});

$('#confirm_tree_dialog').click(function(){
    $('#'+$('#tree_dialog').get(0).dataset.setFor).val($('#selectedTreeNode').val());
});
$('.chooseLocNode').click(function(){
    var inputId = $(this).attr('for'),
        subTreeRoot = $('#'+document.body.dataset.locTreeNodeId).parent(),
        subTreeRootDomCopy = subTreeRoot.get(0).cloneNode(true);
    $(subTreeRootDomCopy).find('.locTreeNode').removeClass().addClass('locTreeNodeInDialog');
    $('#treeInDialog').empty();
    $(subTreeRootDomCopy).appendTo('#treeInDialog');
    $('#treeInDialog').treeview({
        collapsed: true,
        animated: "fast",
    });
    $('#tree_dialog').get(0).dataset.setFor = inputId;
    $('.locTreeNodeInDialog').click(function () {
        var clickedNodeId = parseInt($(this).attr("id"), 10);

        if (clickedNodeId - parseInt(document.body.dataset.locTreeNodeId,10)*100 <100){
            $('#selectedTreeNode').val(clickedNodeId);
        } else {
            alert("Please choose a direct child of clicked node.");
        }
    });
    $('#tree_dialog').draggable();
    $('#tree_dialog').show();
    
});
$('.dialogCloseButton').click(function(){
    $(this).parent().parent().hide();
})

$('.locTreeNode').dblclick(function(){
    document.body.dataset.locTreeNodeId = $(this).attr("id");
    dialog_content = 'Type： LocationTreeNode<br>'+
                     'ID: '+ $(this).attr("id") +'<br>' ;
     
     
     
    var nodeId = $(this).attr("id");
    var nodeName = $(this).text();
    $.get('/loc_tree/nodes/'+nodeId, function(data) {
        if (data.status == '1') {
            alert(data.message);
        }else if (data.status=='0') {
            content = 'Location: ' + data.location + '<br>' +
                       '<button id="addNode">ADD Landmark</button>' +
                       '<button id="delNode">DEL Landmark</button>' +
                       '<button type="button" class="set_node">Save Node Configuration</button><br>'+
                       'Distance Modifiers: '+ data.distanceModifier + '<br>' +
                       'Distance Modifier <button id="changeModifier">Change Modifier</button> ';

            $('#dispTreeNodeInfoBody').html(content);
            $('#changeModifier').click(function(){
               $('#modifier_dialog').draggable();
               $('#modifier_dialog').show();
            });
        }else {
            alert("received JASON format is wrong!!!");
        }
        $('#dispTreeNodeInfo').draggable();
        $('#dispTreeNodeInfo').show();
    });
});
$('.set_node').click(function() {
	var nodeId = $('#SensorId').val(),
	    tmp_type_str = $('#nodeType').val(),
	    direct = $('#direction').val(),
	    nodetype = -1;
	if (!direct) {
	    direct = 'None';
    }
	if (tmp_type_str == "location"){
	    nodetype =0;
    }
	else if (tmp_type_str == "sensor or landmark" || tmp_type_str == "sensor"){
	    nodetype =1;
	}else if (tmp_type_str == "landmark"){
	    nodetype =2;
	}else{
	    nodetype =-1;
	}
    console.log(nodeId);
    console.log($('#locName').val());
    $.ajax('/loc_tree/nodes/' + nodeId, {
        type: 'PUT',
        dataType: 'json',
        data: {type:nodetype, id:$('#SensorId').val(),location: $('#locName').val(),global_coord:$('#gloCoord').val(), 
                size: $('#size').val(), local_coord:$('#localCoord').val(), 
                direction: $('#direction').val(), modifiers:$('#distmod_list').val()},
            
        success: function(data) {
            //data = JSON.parse(data);
            //display update
             $.post('/loc_tree', function(data) {
	    		display_tree(data);
	    		$('#content').append(data.node);
	    		//load_landmark(data.xml);
			});
            if (data.status == 1) {
                alert(data.mesg);
            }
        }
    });
});

function load_landmark(r)
{
	var xml = $.parseXML(r);
	console.log(xml);
	var funi = $(xml).find('landmark');
	for(var i=0; i<funi.length; i++){
		var f = $(funi[i]);
		landmark[f.attr("name")]=f.attr("location");
	};

	for(var l in landmark){
		$('#'+landmark[l]).children('ul',this).append('<li id="'+l+'" role=button class="btn">'+l+'</li>');
	}
}
function add_modifier(){
    var start_id = $('#distmod_start_id').val();
    var end_id = $('#distmod_end_id').val();
    var distance = $('#distmod_distance').val();
    $.ajax({
        url:'/loc_tree/modifier/0',
        data: {start:start_id, end:end_id, distance:distance},
        type:'PUT',
        success: function(data) {
            alert(data.message);
        }
    });
}
function del_modifier(){
    var start_id = $('#distmod_start_id').val();
    var end_id = $('#distmod_end_id').val();
    var distance = $('#distmod_distance').val();
    $.ajax({
        url:'/loc_tree/modifier/1',
        data: {start:start_id, end:end_id, distance:distance},
        type:'PUT',
        success: function(data) {
            alert(data.message);
        }
    });
}
function save_landmark()
{
	var landmark_xml = '<application name="test">\n';
	for(var l in landmark){
		landmark_xml += '	<landmark name="'+ l +'" location="'+landmark[l]+'" />\n';
	}
	landmark_xml += '</application>';	
	console.log(landmark_xml);
	
	$.ajax({
        url:'/loc_tree/save',
        data: {xml:landmark_xml},
        type:'POST',
        success: function(landmark_xml) {
        
        }
    });
}
