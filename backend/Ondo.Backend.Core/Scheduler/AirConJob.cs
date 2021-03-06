﻿using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using Ondo.Backend.Core.Services;
using Quartz;

namespace Ondo.Backend.Core.Scheduler
{
    public class AirConJob : IJob
    {
        private readonly ILogger<AirConJob> _logger;
        private readonly IAirConService _airConService;

        public AirConJob(ILogger<AirConJob> logger, IAirConService airConService)
        {
            _logger = logger;
            _airConService = airConService;
        }

        public async Task Execute(IJobExecutionContext context)
        {
            JobDataMap dataMap = context.JobDetail.JobDataMap;
            string airConId = dataMap.GetString("AirConId");
            bool isAirConOn = dataMap.GetBooleanValue("IsAirConOn");

            var airCon = await _airConService.GetAirCon(airConId);
            airCon.DevicePower = isAirConOn;
            
            _logger.LogInformation($"AirConJob Executed: Location: {airCon.Location}, AirConOn: {airCon.DevicePower}, {context.JobDetail.Description}, {context.Scheduler.SchedulerName}");
            
            await _airConService.ChangeAirConSetting(airCon);
        }
    }
}
