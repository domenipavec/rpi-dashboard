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
}, ['memory'], "Memory");

rpiDashboard.controller('MemoryController', function($scope, $filter) {
    $scope.memory = {
        total: vObject(0),
        used: vObject(0),
        free: vObject(0),
        processes: vObject(0),
        buffers: vObject(0),
        cached: vObject(0)
    };
    
    $scope.swap = {
        total: vObject(0),
        used: vObject(0),
        free: vObject(0)
    };
    
    var ramChart = {
        type: "PieChart",
        displayed: false,
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
                    $scope.memory.free
                ]),
                cObject([
                    vObject("Processes"),
                    $scope.memory.processes
                ]),
                cObject([
                    vObject("File buffers"),
                    $scope.memory.buffers
                ]),
                cObject([
                    vObject("I/O Cached"),
                    $scope.memory.cached
                ])
            ]
        },
        options: {
            backgroundColor: {fill: 'transparent'},
            is3D: true
        }
    };
    var swapChart = {
        type: "PieChart",
        displayed: false,
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
                    $scope.swap.free
                ]),
                cObject([
                    vObject("Used"),
                    $scope.swap.used
                ])
            ]
        },
        options: {
            backgroundColor: {fill: 'transparent'},
            is3D: true
        }
    };    
    var active = true;
    
    $.rpijs.get("memory", function(data) {
        $scope.$apply(function() {
            $scope.memory.total.v = data.total;
            $scope.memory.used.v = data.used;
            $scope.memory.free.v = data.free;
            $scope.memory.free.f = $filter('bytes')(data.free);
            $scope.memory.processes.v = data.processes;
            $scope.memory.processes.f = $filter('bytes')(data.processes);
            $scope.memory.buffers.v = data.buffers;
            $scope.memory.buffers.f = $filter('bytes')(data.buffers);
            $scope.memory.cached.v = data.cached;
            $scope.memory.cached.f = $filter('bytes')(data.cached);
            $scope.ramChart = ramChart;
            $scope.swap.total.v = data.swap.total;
            $scope.swap.free.v = data.swap.free;
            $scope.swap.free.f = $filter('bytes')(data.swap.free);
            $scope.swap.used.v = data.swap.used;
            $scope.swap.used.f = $filter('bytes')(data.swap.used);
            $scope.swapChart = swapChart;
        });
        return active;
    }, {
        update: 5000
    });
    
    $scope.$on('$destroy', function() {
        active = false;
    });
});