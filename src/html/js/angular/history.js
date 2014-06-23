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

historyGraph = function(type, cols, additionalOptions, filter) {
    cols.unshift({
        id: "time",
        label: "Time",
        type: "datetime"
    });
    
    var hg = {};
    
    hg.data = { 
        minute: {
            data: {
                cols: cols,
                rows: [],
            },
            vAxis: {
                ticks: []
            },
            max: 1
        }
    };
    hg.data.hour = angular.copy(hg.data.minute);
    hg.data.day = angular.copy(hg.data.minute);
    hg.data.week = angular.copy(hg.data.minute);
    hg.data.month = angular.copy(hg.data.minute);
    hg.data.year = angular.copy(hg.data.minute);
    
    var options = {
        backgroundColor: {fill: 'transparent'},
        legend: 'none',
        chartArea: {top: 10, width: '75%', height: '85%'},
        vAxis: hg.data.minute.vAxis
    };
    angular.extend(options, additionalOptions);
    hg.graph = {
        type: type,
        data: hg.data.minute.data,
        options: options
    };
    
    var updateVAxis = function(object) {
        var m = Math.pow(2,Math.ceil(Math.log(object.max)/Math.LN2));
        object.vAxis.ticks = [
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

    hg.add = function(data) {
        var row = cObject([
            vObject(new Date())
        ]);
        angular.forEach(data, function(value) {
            row.c.push({
                v: value,
                f: filter(value)
            });
        });
        dataInsert(hg.data.minute, row);
        var d = hg.data.minute.data;
        if (d.rows[d.rows.length-1].c[0].v - d.rows[0].c[0].v > 60000) {
            var row = hg.data.minute.data.rows.shift();
        }
    };
    
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