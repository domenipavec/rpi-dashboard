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

registerPage('/memory', {
    templateUrl: 'partials/memory.html',
    controller: 'MemoryController'
}, ['memory', 'logger'], "Memory");

memoryData = {};

memoryData.memory = {
    total: vObject(0),
    total14: vObject(0),
    total24: vObject(0),
    total34: vObject(0),
    used: vObject(0),
    free: vObject(0),
    processes: vObject(0),
    buffers: vObject(0),
    cached: vObject(0)
};
memoryData.swap = {
        total: vObject(0),
        total14: vObject(0),
        total24: vObject(0),
        total34: vObject(0),
        used: vObject(0),
        free: vObject(0)
};
memoryData.pieChartOptions = {
    backgroundColor: {fill: 'transparent'},
    is3D: true,
    legend: 'none',
    colors: ['#109618', '#dc3912', '#ff9900', '#3366cc'],
    chartArea: {width: '90%', height: '90%'}
};
memoryData.ramChart = {
    type: "PieChart",
    data: {
        cols: [
            {
                id: "label",
                type:"string"
            },
            {
                id: "size",
                type: "number"
            }
        ],
        rows: [
            cObject([
                vObject("Free"),
                memoryData.memory.free
            ]),
            cObject([
                vObject("Processes"),
                memoryData.memory.processes
            ]),
            cObject([
                vObject("File buffers"),
                memoryData.memory.buffers
            ]),
            cObject([
                vObject("I/O Cached"),
                memoryData.memory.cached
            ])
        ]
    },
    options: memoryData.pieChartOptions
};
memoryData.swapChart = {
    type: "PieChart",
    data: {
        cols: [
            {
                id: "label",
                type:"string"
            },
            {
                id: "size",
                type: "number"
            }
        ],
        rows: [
            cObject([
                vObject("Free"),
                memoryData.swap.free
            ]),
            cObject([
                vObject("Used"),
                memoryData.swap.used
            ])
        ]
    },
    options: memoryData.pieChartOptions
};

backgroundUpdate(['memory', 'logger'], 5000, function(done) {
    if (memoryData.ramHistory === undefined) {
        memoryData.ramHistory = historyGraph(
            "AreaChart", 
            [
                {
                    id: "processes",
                    label: "Processes",
                    type: "number"
                },
                {
                    id: "buffers",
                    label: "File buffers",
                    type: "number"
                },
                {
                    id: "cached",
                    label: "I/O Cached",
                    type: "number"
                },
                {
                    id: "free",
                    label: "Free",
                    type: "number"
                }
            ],
            {
                isStacked: true,
                colors: ['#dc3912', '#ff9900', '#3366cc', '#109618']
            },
            bytesFilter,
            "memory/processes|memory/buffers|memory/cached|memory/free"
        );
        
        memoryData.swapHistory = historyGraph(
            "AreaChart",
            [
                {
                    id: "used",
                    label: "Used",
                    type: "number"
                },
                {
                    id: "free",
                    label: "Free",
                    type: "number"
                }
            ],
            {
                isStacked: true,
                colors: ['#dc3912', '#109618'],
            },
            bytesFilter,
            "memory/swap/used|memory/swap/free"
        );
    }

    $.rpijs.get("memory", function(data) {
        if (memoryData.memory.total.v == 0) {
            memoryData.memory.total.v = data.total;
            memoryData.memory.total.f = bytesFilter(data.total);
            memoryData.memory.total14.v = data.total/4;
            memoryData.memory.total14.f = bytesFilter(data.total/4);
            memoryData.memory.total24.v = data.total/2;
            memoryData.memory.total24.f = bytesFilter(data.total/2);
            memoryData.memory.total34.v = data.total*0.75;
            memoryData.memory.total34.f = bytesFilter(data.total*0.75);
            
            memoryData.swap.total.v = data.swap.total;
            memoryData.swap.total.f = bytesFilter(data.swap.total);
            memoryData.swap.total14.v = data.swap.total/4;
            memoryData.swap.total14.f = bytesFilter(data.swap.total/4);
            memoryData.swap.total24.v = data.swap.total/2;
            memoryData.swap.total24.f = bytesFilter(data.swap.total/2);
            memoryData.swap.total34.v = data.swap.total*0.75;
            memoryData.swap.total34.f = bytesFilter(data.swap.total*0.75);
        }
        memoryData.memory.used.v = data.used;
        memoryData.memory.free.v = data.free;
        memoryData.memory.free.f = bytesFilter(data.free);
        memoryData.memory.processes.v = data.processes;
        memoryData.memory.processes.f = bytesFilter(data.processes);
        memoryData.memory.buffers.v = data.buffers;
        memoryData.memory.buffers.f = bytesFilter(data.buffers);
        memoryData.memory.cached.v = data.cached;
        memoryData.memory.cached.f = bytesFilter(data.cached);
        memoryData.ramHistory.add([
            data.processes,
            data.buffers,
            data.cached,
            data.free
        ]);
        memoryData.swap.free.v = data.swap.free;
        memoryData.swap.free.f = bytesFilter(data.swap.free);
        memoryData.swap.used.v = data.swap.used;
        memoryData.swap.used.f = bytesFilter(data.swap.used);
        memoryData.swapHistory.add([
            data.swap.used,
            data.swap.free
        ]);
        done.resolve();
    });
});

rpiDashboard.controller('MemoryController', function($scope, $filter) {
    $scope.memoryData = memoryData;
});