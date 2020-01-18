using System;
using System.Threading.Tasks;
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
            bool turnOn = dataMap.GetBooleanValue("TurnOn");

            var airCon = await _airConService.GetAirCon(airConId);
            airCon.DevicePower = turnOn;
            
            Console.WriteLine("AirConJob Executed");
            _logger.LogInformation("AirConJob Executed!");
            
            await _airConService.ChangeAirConSetting(airCon);
        }
    }
}
