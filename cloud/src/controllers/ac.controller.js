var Client = require('azure-iothub').Client;
var Registry = require('azure-iothub').Registry;
var Ac = require("../models/ac.model");

var connectionString = process.env.AZURE_CONNECTION_STRING;
var targetDevice = 'Ondo-3c71bf3168b1';

var client = Client.fromConnectionString(connectionString);

var registry = Registry.fromConnectionString(connectionString);

exports.getAcSettings = (req, res) => {
    var query = registry.createQuery('SELECT * FROM devices', 100);
    query.nextAsTwin()
        .then(q => {
            var acSettings = new Array();
            q.result.forEach(function (twin) {
                var ac = new Ac(twin.deviceId, twin.properties.desired.status, twin.properties.desired.temperature, twin.properties.desired.quite, twin.properties.desired.powerful, twin.properties.desired.fan);
                acSettings.push(ac.fields);
            });
            res.send(acSettings);
        }).catch(err => {
            res.status(500).send({
                message: err.message || "Some error occurred while retrieving Device Data."
            });
        });
};

exports.sendAcSetting = (req, res) => {
    client.open(function (err) {
        if (err) {
            console.error('could not open IotHub client');
            res.status(500).send({
                message: err.message || "Some error occurred while connecting to Device."
            });
        } else {
            console.log('client opened');

            registry.getTwin(targetDevice, async (err, twin) => {
                if (err) {
                    res.status(500).send({
                        message: err.message || "Some error occurred while getting Device."
                    });
                } else {
                    console.log('Got device twin');
                }

                var twinPatchAc = {
                    properties: {
                        desired: {
                            status: req.body.status,
                            temperature: req.body.temperature,
                            quite: req.body.quite,
                            powerful: req.body.powerful,
                            fan: req.body.fan,
                        }
                    }
                }

                twin.update(twinPatchAc, function (err) {
                    if (err) {
                        res.status(500).send({
                            message: err.message || "Some error occurred while updating properties."
                        });
                    } else {
                        res.status(200).send();
                    }
                });
            });
        }
    });
};
