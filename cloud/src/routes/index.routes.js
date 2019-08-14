var express = require('express');
var ac = require('../controllers/ac.controller')
var sensor = require('../controllers/sensor.controller')
var router = express.Router();
var bodyParser = require('body-parser')

var urlencodedParser = bodyParser.urlencoded({ extended: false })

//Index route
router.get('/', (req, res) =>
    res.send('/sensor => get latest sensor data from all devices, /ac => get current ac settings'))

//Get Sensor Data from all Devices
router.get('/sensor', sensor.readSensorData);

//Send new Ac Settings to Device
router.post('/ac', urlencodedParser, ac.sendAcSetting);

//Get current Ac Settings to Device
router.get('/ac', ac.getAcSettings);

module.exports = router;
