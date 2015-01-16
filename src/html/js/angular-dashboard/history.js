/*  Raspberry Pi Dasboard
 *  =====================
 *  Copyright 2014 Domen Ipavec <domen.ipavec@z-v.si>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

historyGraph = function(type, cols, additionalOptions, filter, loggerValues, preprocess) {
    cols.unshift({
        id: "time",
        label: "Time",
        type: "datetime"
    });
    
    var hg = {};
    
    hg.graphs = [
        {
            name: "minute",
            length: 60,
            step: 1,
            type: type,
            data: {
                cols: cols,
                rows: [],
            },
            options: {
                backgroundColor: {fill: 'transparent'},
                legend: 'none',
                chartArea: {top: 10, left: 70, width: '85%', height: '85%'},
                vAxis: {
                    ticks: []
                }
            },
            max: 1,
        }
    ];
    angular.extend(hg.graphs[0].options, additionalOptions);
    hg.graphs.push(angular.copy(hg.graphs[0]));
    hg.graphs[1].name = "hour";
    hg.graphs[1].length = 3600;
    hg.graphs[1].step = 60;
    hg.graphs.push(angular.copy(hg.graphs[0]));
    hg.graphs[2].name = "day";
    hg.graphs[2].length = 86400;
    hg.graphs[2].step = 1800;
    hg.graphs.push(angular.copy(hg.graphs[0]));
    hg.graphs[3].name = "week";
    hg.graphs[3].length = 604800;
    hg.graphs[3].step = 10800;
    hg.graphs.push(angular.copy(hg.graphs[0]));
    hg.graphs[4].name = "month";
    hg.graphs[4].length = 2592000;
    hg.graphs[4].step = 43200;
    hg.graphs.push(angular.copy(hg.graphs[0]));
    hg.graphs[5].name = "year";
    hg.graphs[5].length = 31536000;
    hg.graphs[5].step = 525600;

    hg.graph = hg.graphs[0];
    
    // calculate ticks based on max value
    var updateVAxis = function(object) {
        if (object.max == 0) {
            object.max = 1;
        }
        var m = Math.pow(2,Math.ceil(Math.log(object.max)/Math.LN2));
        object.options.vAxis.ticks = [
            {
                v: 0,
                f: filter(0, 1)
            },
            {
                v: m/4,
                f: filter(m/4, 1)
            },
            {
                v: m/2,
                f: filter(m/2, 1)
            },
            {
                v: m*0.75,
                f: filter(m*0.75, 1)
            },
            {
                v: m,
                f: filter(m, 1)
            }
        ];
    };
    
    // insert data and update vAxis
    var dataInsert = function(object, row) {
        var update = false;
        if (hg.graph.options.isStacked) {
            value = 0;
            for (i = 1; i < row.c.length; i++) {
                value += row.c[i].v;
            }
            if (value > object.max) {
                object.max = value;
                update = true;
            }
        } else {
            for (i = 1; i < row.c.length; i++) {
                if (row.c[i].v > object.max) {
                    object.max = row.c[i].v;
                    update = true;
                }
            }
        }
        object.data.rows.push(row);
        updateVAxis(object);
    };

    // compare times of object with dt
    var checkTimeDifference = function(object0, object1, dt) {
        if (object0 === undefined) {
            return true;
        }
        if (object1 === undefined) {
            return true;
        }
        return ((object0.c[0].v - object1.c[0].v) >= dt*1000);
    };
    
    // get last item in array
    var getLast = function(array) {
        return array[array.length - 1];
    };
    
    // load graph from logger data
    var updateFromLogger = function(graph) {
        $.rpijs.get("logger/"+graph.name+"?value="+loggerValues, function(data) {
            if (preprocess !== undefined) {
                preprocess(data);
            }
            graph.max = 0;
            if (graph.options.isStacked) {
                graph.max = 0;
                angular.forEach(data, function(values) {
                    for (i = 0; i < values.data.length; i++) {
                        if (values.data[i] !== null) {
                            graph.max += values.data[i];
                            break;
                        }
                    }
                });
                graph.max = Math.round(graph.max);
            } else {
                angular.forEach(data, function(values) {
                    if (values.max > graph.max) {
                        graph.max = values.max;
                    }
                });
            }
            graph.data.rows = [];
            var firstKey = loggerValues.split(/\||~/)[0].replace(/\//g, "-");
            var now = data[firstKey].start;
            for (i = 0; now <= data[firstKey].end; i++, now += data[firstKey].step) {
                var row = cObject([
                    vObject(new Date(now*1000)),
                ]);
                angular.forEach(data, function(values) {
                    row.c.push(vObject(values.data[i], filter));
                });
                graph.data.rows.push(row);
            }
            updateVAxis(graph);
            graph.options.hAxis = {
                viewWindow: {
                    min: new Date(data[firstKey].start*1000),
                    max: new Date(data[firstKey].end*1000)
                }
            };
        });
    };
    
    // add data to graph (default minute)
    hg.add = function(data) {
        row = cObject([
            vObject(new Date())
        ]);
        angular.forEach(data, function(value) {
            row.c.push(vObject(value, filter));
        });
        dataInsert(hg.graphs[0], row);
        var d = hg.graphs[0].data;
        if (checkTimeDifference(getLast(d.rows), d.rows[0], hg.graphs[0].length)) {
            d.rows.shift();
        }
        if (checkTimeDifference(getLast(d.rows), getLast(hg.graph.data.rows), hg.graph.step)) {
            updateFromLogger(hg.graph);
        }
    };
    
    updateFromLogger(hg.graphs[1]);
    updateFromLogger(hg.graphs[2]);
    updateFromLogger(hg.graphs[3]);
    updateFromLogger(hg.graphs[4]);
    updateFromLogger(hg.graphs[1]);

    return hg;
};

rpiDashboard.directive("historyChart", function() {
    return {
        scope: {
            hg: "=chart"
        },
        templateUrl: "partials/history.html"
    };
});