var landmark = {};
$(document).ready(function() {

$("#addLandmark").click(function () {
	var name = $('#landmarkNameInput').val(),
	    coord = $('#landmarkCoordInput').val();
	landmark[name] = coord;
	$.ajax('/loc_tree/land_mark', {
        type: 'PUT',
        dataType: 'json',
        data: {ope: 1, name: name, location: document.body.dataset.currentLocation,
            coordinate:coord, size:0, direction:0},
        success: function (data) {
            //data = JSON.parse(data);
            //display update
             $.post('/loc_tree', function(data) {
	    		display_tree("#content",data);
	    		$('#content').append(data.node);
			});
            if (data.status === 1) {
                alert(data.mesg);
            }
        }
    });
});

$("#delLandmark").click(function(){
	var name = $('#landmarkNameInput').val();
	$.ajax('/loc_tree/land_mark', {
        type: 'PUT',
        dataType: 'json',
        data: {ope: 0, name: name, location: document.body.dataset.currentLocation,
            coordinate: '(0,0,0)', size:0, direction:0},
        success: function(data) {
            //data = JSON.parse(data);
            //display update
             $.post('/loc_tree', function(data) {
	    		display_tree("#content", data);
            	$("li").remove("#"+name);
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
			});
            if (data.status == 1) {
                alert(data.mesg);
            }
        }
    });
});
*/
$("#loadTree").click(function () {
    load_tree();
});
$("#saveTree").click(function(){
	save_tree();
});
$("#addModifier").click(function(){
    add_modifier();
    $('#'+document.body.dataset.locTreeNodeId).dblclick();
});
$("#delModifier").click(function(){
    del_modifier();
    $('#'+document.body.dataset.locTreeNodeId).dblclick();
});
$('.sensor_node_btn').click(function(){
   //dialog_content not using now
    dialog_content = '<button id="openLandmarkEditor">Add/Del Landmark</button>'+
     //'<button id="delNode">DEL Landmark</button>'+'<button id="addNode">ADD Landmark</button>'+'<button type="button" class="set_node">Save Node Configuration</button><br>'+
     'Distance Barrier <button id="addModifier">Add Distance Barrier</button> '+ 
      '<button id="delModifier">Delete Distance Barrier</button><br>' +
      'Existing Modifiers <div id="distmod_list"></div><br>' +
      'start treenode ID<input id = "distmod_start_id" type=text size="20"><br>'+
       'end treenode ID<input id = "distmod_end_id" type=text size="20"><br>'+
       'distance<input id = "distmod_distance" type=text size="20"><br>';
    var nodeId = $(this).attr("id");
    var nodeName = $(this).text();
    var locTreeNodeId = $(this).parent().parent().children('.locTreeNode').attr("id")
                        ||$(this).parent().parent().children('.landmarkTreeNode').attr("id");
	$('#node_addDel').val(nodeId);
	$.get('/loc_tree/nodes/'+locTreeNodeId+'/'+nodeId, function(data) {
        if (data.status == '1') {
            alert(data.message);
        }else if (data.status=='0'){
            
            if (nodeId[0]=='l'){
                $('#node_info_dialog_title').html("Landmark Information");
                $('#node_info_dialog_body').html("Location: "+ data.location +"<br>" +
                                                 "Landmark Name: " + nodeName.substring(0,nodeName.indexOf('(')));
            }else{
                $('#node_info_dialog_title').html("WuNode Information");
                $('#node_info_dialog_body').html("Location: "+ data.location +"<br>" +
                                                 "WuNode ID: " + nodeId.substring(2));
            }
            $('#node_info_dialog').draggable();
            $('#node_info_dialog').show();
        }else{
            alert("received JASON format is wrong!!!");
        }
    });
});

$('#confirm_tree_dialog').click(function(){
    $('#'+$('#tree_dialog').get(0).dataset.setFor).val($('#selectedTreeNode').val());
});


$('.chooseLocNode').click(function promptChooseLocation(){
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
            $('#selectedTreeNode').val($(this).text());
            $('#'+$('#tree_dialog').get(0).dataset.setFor).attr("loc_node_id", clickedNodeId);
        } else {
            alert("Please choose a direct child of clicked node.");
        }
    });
    $('#tree_dialog').draggable();
    $('#tree_dialog').show();  
});

$('.dialogCloseButton').click(function(){
    $(this).parent().parent().hide();
});

$('.locTreeNode').dblclick(function locTreeNodeHandler(){
    document.body.dataset.locTreeNodeId = $(this).attr("id");
    dialog_content = 'Type： LocationTreeNode<br>'+
                     'ID: '+ $(this).attr("id") +'<br>' ;
     
    var nodeId = $(this).attr("id");
    var nodeName = $(this).text();
    $.get('/loc_tree/nodes/'+nodeId, function (data) {
        if (data.status == '1') {
            alert(data.message);
        }else if (data.status=='0') {
            document.body.dataset.currentLocation = data.location;
            content = 'Location: ' + data.location + '<br><br>' +
           //            '<button type="button" class="set_node">Save Node Configuration</button><br>'+
                       'Existing Distance Barriers: '+ data.distanceModifier + '<br>';
            footer_content = '<button id="changeModifier">Edit Distance</button>  &nbsp;' +
                            '<button class="dialogCloseButton" data-dismiss="modal" aria-hidden="true">Close</button>'
            
            $('#dispTreeNodeInfoFooter').html(footer_content);
            $('#dispTreeNodeInfoBody').html(content);
            
            $('#changeModifier').click(function(){
                $("#landmark_dialog_body").html('Under Location: ' + data.location + '<br>');
                $('#modifier_dialog').draggable();
                $('#modifier_dialog').show();
            });
            $('.dialogCloseButton').click(function(){
                $(this).parent().parent().hide();
            });
        }else {
            alert("received JASON format is wrong!!!");
        }
        $('#dispTreeNodeInfo').draggable();
        $('#dispTreeNodeInfo').show();
    });
});

$('.landmarkTreeNode').dblclick(function landmarkTreeNodeHandler() {
    document.body.dataset.locTreeNodeId = $(this).attr("id");
    dialog_content = 'Type： LocationTreeNode<br>'+
                     'ID: '+ $(this).attr("id") +'<br>' ;
     
    var nodeId = $(this).attr("id");
    var nodeName = $(this).text();
    $.get('/loc_tree/nodes/'+nodeId, function (data) {
        if (data.status == '1') {
            alert(data.message);
        }else if (data.status=='0') {
            document.body.dataset.currentLocation = data.location;
            content = 'Location: ' + data.location + '<br><br>' ;
            footer_content = '<button id="openLandmarkEditor">Edit Landmark</button>  &nbsp;' +
                      '<button class="dialogCloseButton" data-dismiss="modal" aria-hidden="true">Close</button>'
            
            $('#dispTreeNodeInfoFooter').html(footer_content);
            $('#dispTreeNodeInfoBody').html(content);
            $("#openLandmarkEditor").click( function openLandmarkEditorHandler() {
                $('#landmark_dialog').draggable();
                $('#landmark_dialog').show();
            });
            $('.dialogCloseButton').click(function(){
                $(this).parent().parent().hide();
            });
        }else {
            alert("received JASON format is wrong!!!");
        }
        $('#dispTreeNodeInfo').draggable();
        $('#dispTreeNodeInfo').show();
    });
})
$('.set_node').click(function set_nodeHandler() {
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
        data: {type:nodetype, id:$('#SensorId').val(),location: $('#locName').val(),
                size: $('#size').val(), local_coord:$('#localCoord').val(), 
                direction: $('#direction').val(), modifiers:$('#distmod_list').val()},
            
        success: function(data) {
            //data = JSON.parse(data);
            //display update
             $.post('/loc_tree', function(data) {
	    		display_tree("#content",data);
	    		$('#content').append(data.node);
			});
            if (data.status == 1) {
                alert(data.mesg);
            }
        }
    });
});

});


function add_modifier(){
    var start_id = $('#distmod_start_id').attr("loc_node_id");
    var end_id = $('#distmod_end_id').attr("loc_node_id");
    var distance = $('#distmod_distance').val();
    $.ajax({
        url:'/loc_tree/modifier/0',
        data: {start:start_id, end:end_id, distance:distance},
        type:'PUT',
        success: function(data) {
            if (data.status == 1) {
                alert(data.message);
            }
        }
    });
}
function del_modifier(){
    var start_id = $('#distmod_start_id').attr("loc_node_id");
    var end_id = $('#distmod_end_id').attr("loc_node_id");
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
function load_tree(r)
{
	$.ajax({
        url:'/loc_tree/load',
        type:'POST',
        success: function(data) {
            alert(data.message);
            $.post('/loc_tree', function(data) {
                var container_name = "#content";
                if (document.body.dataset.installer_mode == "true") {
                    container_name = "#location_editor"
                }
	    		display_tree(container_name,data);
	    		$(container_name).append(data.node);
			});
        }
    });
}
function save_tree()
{
	
	$.ajax({
        url:'/loc_tree/save',
        type:'POST',
        success: function(data) {
            alert(data.message);
        }
    });
}

//generate the tree for landmark or for location tree editor
//node class could be locTreeNode, landmarkTreeNode, locTreeNodeInDialog
//corresponding to different click actions
function generate_tree(rt, node_class) {
    var node_data = JSON.parse(rt.loc), //refer to _getJson in class LocationTreeNode of locationTree.py
        tree_level = 0,
        html_tree = '';
    
    html_tree += '<ul id="display" class="treeview">';
    for(var i=0; i<node_data.length;i++){
          if(node_data[i][1] == tree_level){
              //do nothing
          }else if(node_data[i][1] > tree_level){
            html_tree +='<ul>';
            tree_level = node_data[i][1];
        }else if (node_data[i][1]<tree_level){
          for(var j=0; j<tree_level-node_data[i][1] ;j++){
                  html_tree += '</ul></li>';
              }
              tree_level = node_data[i][1];
        }
        //see locationTree.py, class locationTree.toJason() function for detailed data format
          if(node_data[i][0] === 0){ //this is a tree node
            if (node_data[i][1] === 0){  //root
                  html_tree += '<li id="'+ node_data[i][2][1] +'"><button class="'+ node_class +'" id='+node_data[i][2][0]+'>'+node_data[i][2][1]+'</button>';
            }else{
                for (var j=i+1;j<node_data.length;++j) {
                    if (node_data[j][1]==node_data[i][1]){
                        html_tree += '<li  id="'+ node_data[i][2][1] +'"><button class="'+ node_class +'" id='+node_data[i][2][0]+'>'+node_data[i][2][1]+'</button>';
                        break;
                    }
                    if (node_data[j][1]<node_data[i][1]){
                        html_tree += '<li  id="'+ node_data[i][2][1] +'"><button class="'+ node_class +'" id='+node_data[i][2][0]+'>'+node_data[i][2][1]+'</button>';
                        break;
                    }
                    if (j==node_data.length-1){
                        html_tree += '<li id="'+ node_data[i][2][1] +'"><button class="'+ node_class +'" id='+node_data[i][2][0]+'>'+node_data[i][2][1]+'</button>';
                    }
                }
            }
          }else if (node_data[i][0] == 1){            //this is a sensor
                html_tree += '<li id=se'+node_data[i][2][0]+' data-toggle=modal  role=button class="sensor_node_btn btn" >'+node_data[i][2][0]+node_data[i][2][1]+'</li>';
          }else if(node_data[i][0]==2){               //this is a landmark
                html_tree += '<li id=lm'+node_data[i][2][1]+' data-toggle=modal  role=button class="sensor_node_btn btn" >'+node_data[i][2][0]+node_data[i][2][2]+'</li>';
          }
        }
    html_tree += '</td><td valign="top">';
    return html_tree;
}

//display_tree("#content", rt)
function display_tree(container_id,rt) {
    $(container_id).empty();
    $(container_id).append('<script type="text/javascript" src="/static/js/jquery.treeview.js"></script>'+
        '<script type="text/javascript" src="/static/js/tree_expand.js"></script>'
        );
    html_tree = '';
    html_tree = '<table width="100%">';
    html_tree += '<tr><td width="5%"></td><td></td><td></td></tr>';
    html_tree += '<tr><td></td><td>';
    html_tree += '</td><td valign="top">';
    if (container_id == "#content"){    //#content means in user mode, generating for landmark edit
        html_tree += generate_tree(rt, "landmarkTreeNode");
        html_tree += '<button id="saveTree">Save Landmarks</button><br>';
        //+'<button id="loadTree">Load Saved Distance Barriers Landmarks</button>';
    } else {                            //otherwise installer mode, generating for modifier edit
        html_tree += generate_tree(rt, "locTreeNode");
        html_tree += '<button id="saveTree">Save Distance Barriers</button><br>';
        //+'<button id="loadTree">Load Saved Distance Barriers Landmarks</button>';
    }  
    html_tree += '</td></tr></table>';
    $(container_id).append(html_tree);
    $("#display").treeview({
        collapsed: true,
        unique: true
    });
}
