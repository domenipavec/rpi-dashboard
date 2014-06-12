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

registerPage('/network', {
    templateUrl: 'partials/network.html',
    controller: 'NetworkController'
}, ['network'], "Network");

networkData = {};
networkData.list = {};
networkData.ready = false;
networkData.historyPrototype = {
    type: "AreaChart",
    data: {
        cols: [
            {
                id: "time",
                label: "Time",
                type: "datetime"
            },
            {
                id: "tx",
                label: "Transmit",
                type: "number"
            },
            {
                id: "rx",
                label: "Receive",
                type: "number"
            }
        ],
        rows: []
    },
    options: {
        backgroundColor: {fill: 'transparent'},
        legend: 'none',
        chartArea: {top: 10, width: '75%', height: '85%'},
        vAxis: {
            ticks: []
        },
        lineWidth: 1
    }
};
networkData.updateVAxis = function(name) {
    var max = networkData.list[name].historyMax;
    networkData.list[name].history.options.vAxis.ticks = [
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
networkData.throughputRequestOptions = {
    rate: true,
    format: []
};
backgroundUpdate(['network'], 1000, function(done) {
    if (!networkData.ready) {
        $.rpijs.get("network/list", function(data) {
            angular.forEach(data, function(value, name) {
                networkData.list[name] = value;
                networkData.throughputRequestOptions.format.push({
                    key: ["rx", name],
                    rate: true
                });
                networkData.throughputRequestOptions.format.push({
                    key: ["tx", name],
                    rate: true
                });
                networkData.list[name].throughput = {
                    rx: vObject(0),
                    tx: vObject(0)
                };
                networkData.list[name].history = angular.copy(networkData.historyPrototype);
                networkData.list[name].historyMax = 0;
            });
            networkData.ready = true;
            done.resolve();
        });
        return;
    }
    $.rpijs.get("network/bytes", function(data) {
        angular.forEach(data.rx, function(value, name) {
            networkData.list[name].throughput.rx.v = value;
            networkData.list[name].throughput.rx.f = bpsFilter(value);
            if (value > networkData.list[name].historyMax) {
                networkData.list[name].historyMax = value;
                networkData.updateVAxis(name);
            }
        });
        angular.forEach(data.tx, function(value, name) {
            networkData.list[name].throughput.tx.v = value;
            networkData.list[name].throughput.tx.f = bpsFilter(value);
            if (value > networkData.list[name].historyMax) {
                networkData.list[name].historyMax = value;
                networkData.updateVAxis(name);
            }
            networkData.list[name].history.data.rows.push(cObject([
                vObject(new Date()),
                angular.copy(networkData.list[name].throughput.tx),
                angular.copy(networkData.list[name].throughput.rx)
            ]));
        });
        done.resolve();
    }, networkData.throughputRequestOptions);
});

rpiDashboard.controller('NetworkController', function($scope) {
    $scope.list = networkData.list;
});
