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
    path: '/spi',
    route: {
        templateUrl: 'partials/spi.html',
        controller: 'SPIController'
    },
    accessDependencies: {spi: 'write'},
    title: "SPI",
    description: "Simultaneously write and read data on SPI bus."
});

rpiDashboard.controller("SPIController", ['$scope', function($scope) {
    $scope.bus = 0;
    $scope.read = "";
    $scope.write = "";
    $scope.finished = false;

    $scope.execute = function() {
        $.rpijs.post("spi/"+$scope.bus, $scope.write, function(data) {
            if (data == "SPI setup problem." || data == "SPI bus write failed") {
                $scope.status = "warning";
                $scope.statusText = data;
            } else {
                $scope.status = "success";
                $scope.statusText = "Successful!";
                $scope.read = data;
            }
            $scope.finished = true;
        });
    };
    
    $scope.buttonClick = function() {
        $scope.finished = false;
        $scope.read = "";
    };
    
    $scope.busChange = function() {
        $.rpijs.get("spi/"+$scope.bus+"/frequency", function(data) {
            $scope.frequency = data;
        });
    };
    
    $scope.frequencyStatus = true;    
    $scope.frequencyChange = function() {
        if (this.frequency === null) {
            return;
        }
        if (typeof this.frequency != "number") {
            this.frequency = 500000;
        }
        $.rpijs.post("spi/"+$scope.bus+"/frequency", $scope.frequency, function(data) {
            if (data == "Successful!") {
                $scope.frequencyStatus = true;
            } else {
                $scope.frequencyStatus = false;
            }
        });
    };
    
    $scope.busChange();
}]);
