using System;

namespace Ondo.Api.Scheduler
{
    public class JobSchedule
    {
        public JobSchedule(Type jobType, bool turnOn, string cronExpression, string airConId)
        {
            JobType = jobType;
            TurnOn = turnOn;
            CronExpression = cronExpression;
            AirConId = airConId;
        }
        
        public Type JobType { get; }

        public bool TurnOn { get; }

        public string CronExpression { get; }

        public string AirConId { get; }
    }
}
