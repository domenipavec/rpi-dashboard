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

registerPage('/storage', {
    templateUrl: 'partials/storage.html',
    controller: 'StorageController'
}, ['storage'], "Storage");

storageData = {};
storageData.throughputRequest = {
    rate: true,
    format: []
};
storageData.throughputMax = 0;
storageData.updateVAxis = function() {
    var max = storageData.throughputMax;
    storageData.throughputHistory.options.vAxis.ticks = [
        {
            v: 0,
            f: bpsFilter(0)
        },
        {
            v: max/4,
            f: bpsFilter(max/4)
        },
        {
            v: max/2,
            f: bpsFilter(max/2)
        },
        {
            v: max*0.75,
            f: bpsFilter(max*0.75)
        },
        {
            v: max,
            f: bpsFilter(max)
        }
    ];
};
storageData.storageTable = {
    type: "Table",
    data: {
        cols: [
            {
                id: "device",
                label: "Device",
                type: "string"
            },
            {
                id: "mount",
                label: "Mount point",
                type: "string"
            },
            {
                id: "filesystem",
                label: "Filesystem",
                type: "string"
            },
            {
                id: "size",
                label: "Size",
                type: "number"
            },
            {
                id: "use",
                label: "Used",
                type: "number"
            }
        ],
        rows: []
    },
    options: {
    },
    formatters: {
        bar: [
            {
                columnNum: 4,
                max: 1
            }
        ]
    }
};
storageData.throughputHistory = {
    type: "LineChart",
    data: {
        cols: [
            {
                id: "time",
                label: "Time",
                type: "datetime"
            },
        ],
        rows: []
    },
    options: {
        backgroundColor: {fill: 'transparent'},
        chartArea: {top: 10, width: '75%', height: '85%'},
        vAxis: {}
    }
};
backgroundUpdate(['storage'], 1000, function(done) {
    if (storageData.throughputRequest.format.length == 0) {
        done.resolve();
        return;
    }
    $.rpijs.get("storage/throughput", function(data) {
        var row = cObject([
            vObject(new Date())
        ]);
        angular.forEach(data, function(item, name) {
            var value = vObject(item.total);
            value.f = bpsFilter(item.total);
            if (item.total > storageData.throughputMax) {
                storageData.throughputMax = item.total;
                storageData.updateVAxis();
            }
            row.c.push(value);
        });
        storageData.throughputHistory.data.rows.push(row);
        done.resolve();
    }, storageData.throughputRequest);
});

rpiDashboard.controller('StorageController', function($scope) {
    storageData.storageTable.data.rows = [];
    $.rpijs.get("storage/list", function(data) {
        $scope.$apply(function() {
            angular.forEach(data, function(item) {
                var size = vObject(item.size);
                size.f = bytesFilter(item.size);
                var use = vObject(item.use);
                use.f = procentsFilter(item.use);
                storageData.storageTable.data.rows.push(cObject([
                    vObject(item.device),
                    vObject(item.mount),
                    vObject(item.filesystem),
                    size,
                    use
                ]));
            });
            $scope.storageTable = storageData.storageTable;
        });
    });

    if (storageData.throughputRequest.format.length == 0) {
        $.rpijs.get("storage/throughput", function(data) {
            angular.forEach(data, function(item, name) {
                storageData.throughputHistory.data.cols.push({
                    id: name,
                    label: name,
                    type: "number"
                });
                storageData.throughputRequest.format.push({
                    key: [name, "total"],
                    rate: true
                });
            });
        });
    }
    
    $scope.throughputHistory = storageData.throughputHistory;
});
