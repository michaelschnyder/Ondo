using System;

namespace Ondo.Backend.Core.Scheduler
{
    public class JobSchedule
    {
        public JobSchedule(Type jobType, bool isAirConOn, string cronExpression, string airConId)
        {
            JobType = jobType;
            IsAirConOn = isAirConOn;
            CronExpression = cronExpression;
            AirConId = airConId;
        }
        
        public Type JobType { get; }

        public bool IsAirConOn { get; }

        public string CronExpression { get; }

        public string AirConId { get; }
    }
}
