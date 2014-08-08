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

registerPage('/i2c', {
    templateUrl: 'partials/i2c.html',
    controller: 'I2CController'
}, ['i2c'], "I2C", "Read and write to devices on I2C bus.");

rpiDashboard.controller("I2CController", function($scope) {
    $scope.address = 96;
    $scope.actions = [
        {
            id: 0,
            text: "Read"
        },
        {
            id: 1,
            text: "Write"
        },
        {
            id: 2,
            text: "Read 8-bit register"
        },
        {
            id: 3,
            text: "Write 8-bit register"
        },
        {
            id: 4,
            text: "Read 16-bit register"
        },
        {
            id: 5,
            text: "Write 16-bit register"
        }
    ];
    $scope.action = $scope.actions[0];
    $scope.register = 0;
    $scope.registerShow = function() {
        return $scope.action.id > 1;
    };
    $scope.value = 0;
    $scope.valueShow = function() {
        return $scope.action.id % 2 == 1;
    };
    $scope.valueMax = function() {
        if ($scope.action.id > 3) {
            return 65535;
        } else {
            return 255;
        }
    };
    $scope.buttonText = function() {
        if ($scope.action.id % 2 == 1) {
            return "Write!";
        } else {
            return "Read!";
        }
    };
    $scope.finished = false;
    
    var printStatus = function(data) {
        $scope.$apply(function() {
            if (data == "Successful!") {
                $scope.status = "success";
            } else {
                $scope.status = "warning";
            }
            $scope.statusText = data;
            $scope.finished = true;
        });
    };
    
    var printValue = function(data) {
        $scope.$apply(function() {
            if (typeof data == "number") {
                if (data < 0) {
                    $scope.statusText = "Unsuccessful read!";
                    $scope.status = "warning";
                } else {
                    $scope.statusText = "Value: " + data;
                    $scope.status = "success";
                }
            } else {
                $scope.statusText = "Invalid address!";
                $scope.status = "warning";
            }
            $scope.finished = true;
        });
    }
    
    $scope.execute = function() {
        switch ($scope.action.id) {
            case 0:
                $.rpijs.get("i2c/byte?address="+$scope.address, printValue);
                break;
            case 1:
                $.rpijs.post("i2c/byte?address="+$scope.address, $scope.value, printStatus);
                break;
            case 2:
                $.rpijs.get("i2c/register8/"+$scope.register+"?address="+$scope.address, printValue);
                break;
            case 3:
                $.rpijs.post("i2c/register8/"+$scope.register+"?address="+$scope.address, $scope.value, printStatus);
                break;
            case 4:
                $.rpijs.get("i2c/register16/"+$scope.register+"?address="+$scope.address, printValue);
                break;
            case 5:
                $.rpijs.post("i2c/register16/"+$scope.register+"?address="+$scope.address, $scope.value, printStatus);
                break;
        }
    };
    
    $scope.buttonClick = function() {
        $scope.finished = false;
    };
});