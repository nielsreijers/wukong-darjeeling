

$(function() {
    window.options = {repeat: true};

    $('a#nodes-btn').click(function(e) {
        console.log('refresh nodes');
        $(this).tab('show');

        $('#nodes').block({
            message: '<h1>Processing</h1>',
            css: { border: '3px solid #a00' }
        });
        $.post('/nodes/refresh', function(data) {
            $('#nodes').html(data.nodes);
            $('#nodes').unblock();
        });
    });

    $('a#mapping_results-btn').click(function(e) {
        console.log('map result');
        $.post('/applications/' + current_application + '/deploy/map', function(data) {
            // Already an object
            if (data.status == 1) {
                alert(data.mesg);
            } else {
                var $table = $('#mapping_results table tbody');
                $table.empty();

                //console.log('data.mapping_results');
                //console.log(data.mapping_results);

                _.each(data.mapping_results, function(result) {
                    if (result.instances.length > 0) {
                        _.each(result.instances, function(instance) {
                            if (instance.portNumber) {
                                $table.append(_.template('<tr class=success><td><%= instanceId %></td><td><%= name %></td><td><%= nodeId %></td><td><%= portNumber %></td></tr>')(instance));
                            } else {
                                $table.append(_.template('<tr class=info><td><%= instanceId %></td><td><%= name %></td><td><%= nodeId %></td><td><%= portNumber %></td></tr>')(instance));
                            }
                        });
                    } else {
                        $table.append(_.template('<tr class=error><td><%= instanceId %></td><td><%= name %></td><td>Cannot find matching wuobjects</td><td></td></tr>')(result));
                    }
                });
            }
        });
    });

    // Actually deploy
    $('a#deploy-btn').click(function(e) {
        e.preventDefault();
        $(this).tab('show');
        $.post('/applications/' + current_application + '/deploy', function(data) {
            // Already an object
            console.log('deploy signal set');
            if (data.status == 1) {
                alert(data.mesg);
            } else {
                $('#deploy_results').dialog({modal: true, autoOpen: true, width: 600, height: 300}).dialog('open').bind('dialogclose', function(event, ui) {
                    $.post('/applications/' + current_application + '/reset');
                    $('#deploy_results').dialog("close");
                });
            }
        });
    });
});
