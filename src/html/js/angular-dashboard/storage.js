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

registerPage({
    path: '/storage',
    route: {
        templateUrl: 'partials/storage.html',
        controller: 'StorageController'
    },
    accessDependencies: ['storage', 'logger'],
    title: "Storage",
    description: "Show storage mount points with usage and throughput information with history graphs."
});

registerWidget(5, ['$scope', function($scope) {
        $scope.storageRoot = angular.copy(memoryData.swapChart);
        $scope.storageRoot.data.rows = storageData.rootFS;
        $scope.storageRootTotalSize = storageData.rootTotalSize;
    }], 
    "partials/widgets/storage.html", ['storage']);

storageData = {};
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
                id: "used",
                label: "Used",
                type: "number"
            },
            {
                id: "use",
                label: "Use",
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
                columnNum: 5,
                max: 1
            }
        ]
    }
};
storageData.throughputRequest = {
    rate: true,
    format: []
};
storageData.rootFS = [
    cObject([
        vObject("Free")
    ]),
    cObject([
        vObject("Used")
    ])
];
storageData.rootTotalSize = vObject();

backgroundUpdate(['storage', 'logger'], 1000, function(done) {
    if (storageData.throughputRequest.format.length == 0) {
        $.rpijs.get("storage/throughput", function(data) {
            var columns = [];
            var values = "";
            angular.forEach(data, function(item, name) {
                columns.push({
                    id: name,
                    label: name,
                    type: "number"
                });
                if (values.length != 0) {
                    values += "~";
                }
                values += "storage-throughput-"+name+"-total";
                storageData.throughputRequest.format.push({
                    key: [name, "total"],
                    rate: true
                });
            });
            storageData.throughputHistory = historyGraph(
                "LineChart",
                columns,
                {
                    legend: 'right',
                    chartArea: {
                        top: 10,
                        left: 80,
                        width: '65%',
                        height: '85%'
                    }
                },
                bpsFilter,
                values
            );
            done.resolve();
        });
        return;
    }

    $.rpijs.get("storage/throughput", function(data) {
        var values = [];
        angular.forEach(data, function(item, name) {
            values.push(item.total);
        });
        storageData.throughputHistory.add(values);
        done.resolve();
    }, storageData.throughputRequest);
});

backgroundUpdate(['storage', 'logger'], 60000, function(done) {
    $.rpijs.get("storage/list", function(data) {
        storageData.storageTable.data.rows = [];
        angular.forEach(data, function(item) {
            if (item.mount == '/') {
                storageData.rootFS[0].c[1] = vObject(item.size-item.used, bytesFilter);
                storageData.rootFS[1].c[1] = vObject(item.used, bytesFilter);
                storageData.rootTotalSize.v = item.size;
            }
            storageData.storageTable.data.rows.push(cObject([
                vObject(item.device),
                vObject(item.mount),
                vObject(item.filesystem),
                vObject(item.size, bytesFilter),
                vObject(item.used, bytesFilter),
                vObject(item.use, procentsFilter)
            ]));
        });
        done.resolve();
    });
});

rpiDashboard.controller('StorageController', ['$scope', function($scope) {
    $scope.storageData = storageData;
}]);
