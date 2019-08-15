var schedule = require('node-schedule');
var acController = require('../controllers/ac.controller');
var ac = require('../data/ac');

//Everyday at 11pm
var ruleEverydayAt11pm = new schedule.RecurrenceRule();
ruleEverydayAt11pm.dayOfWeek = 0 - 6;
ruleEverydayAt11pm.hour = 23;

//Everyday at 10am
var ruleEverydayAt10am = new schedule.RecurrenceRule();
ruleEverydayAt10am.dayOfWeek = 0 - 6;
ruleEverydayAt10am.hour = 10;

//Everyday at 2am
var ruleEverydayAt2am = new schedule.RecurrenceRule();
ruleEverydayAt2am.dayOfWeek = 0 - 6;
ruleEverydayAt2am.hour = 2;

//Everyday at 6am
var ruleEverydayAt6am = new schedule.RecurrenceRule();
ruleEverydayAt6am.dayOfWeek = 0 - 6;
ruleEverydayAt6am.hour = 6;

exports.scheduledJobs = () => {
    //MasterRoom
    schedule.scheduleJob(ruleEverydayAt11pm, function () { TurnOnAc(ac.MASTER_ROOM.fields.deviceId) });
    schedule.scheduleJob(ruleEverydayAt10am, function () { TurnOffAc(ac.MASTER_ROOM.fields.deviceId) });

    //GuestRoom
    schedule.scheduleJob(ruleEverydayAt11pm, function () { TurnOffAc(ac.GUEST_ROOM.fields.deviceId) });

    //DiningRoom
    schedule.scheduleJob(ruleEverydayAt11pm, function () { TurnOffAc(ac.DINING_ROOM.fields.deviceId) });

    //LivingRoom 
    schedule.scheduleJob(ruleEverydayAt2am, function () { TurnOffAc(ac.LIVING_ROOM.fields.deviceId) });
    schedule.scheduleJob(ruleEverydayAt6am, function () { TurnOnAc(ac.LIVING_ROOM.fields.deviceId) });
}

function TurnOnAc(deviceId) {
    console.log(`Device with DeviceId: ${deviceId}, Turned On Ac`);
    var devicePower = 1;
    var targetTempC = 24;
    var quiteOn = true;
    var powerfulOn = true;
    var fanMode = true;
    var swingVOn = true;
    var swingHOn = true;
    acController.SendAcSettingToAzure(deviceId, devicePower, targetTempC, quiteOn, powerfulOn, fanMode, swingVOn, swingHOn);
}

function TurnOffAc(deviceId) {
    console.log(`Device with DeviceId: ${deviceId}, Turned Off Ac`)
    var devicePower = 0;
    var targetTempC = 24;
    var quiteOn = true;
    var powerfulOn = true;
    var fanMode = true;
    var swingVOn = true;
    var swingHOn = true;
    acController.SendAcSettingToAzure(deviceId, devicePower, targetTempC, quiteOn, powerfulOn, fanMode, swingVOn, swingHOn);
}
