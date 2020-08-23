using Microsoft.Azure.Cosmos.Table;
using System;

namespace Ondo.Backend.Core.Model
{
    public class JobScheduleEntity : TableEntity
    {
        public bool IsJobOn { get; set; }

        public Type JobType { get; set; }

        public bool IsAirConOn { get; set; }

        public string CronExpression { get; set; }

        public string AirConId { get; set; }

        public string Location { get; set; }
    }
}
