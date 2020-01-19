using System;
using Microsoft.Extensions.DependencyInjection;
using Ondo.Backend.Core.Scheduler;
using Ondo.Backend.Core.Services;
using Ondo.Mvc;
using Quartz;
using Quartz.Impl;
using Quartz.Spi;

namespace Ondo.Backend.Core
{
    public static class OndoCoreFeature
    {
        public static void ConfigureOndoCore(this IServiceCollection services)
        {
            //Add validator
            services.AddSingleton<IDeviceService, DevicesService>();
            services.AddSingleton<IAirConService, AirConService>();

            // Add Quartz services
            services.AddSingleton<IJobFactory, JobFactory>();
            services.AddSingleton<ISchedulerFactory, StdSchedulerFactory>();

            // Add our job
            services.AddSingleton<AirConJob>();

            const string GuestRoomAirconId = "Ondo-3c71bf316a40";
            const string MasterRoomAirconId = "Ondo-3c71bf316563";
            const string DiningRoomAirconId = "Ondo-3c71bf3168b1";
            const string LivingRoomAirconId = "Ondo-3c71bf315b91";

            //Everyday at 10pm
            const string RuleEverydayAt10pm = "0 0 22 ? * *";

            //Weekend at 10am
            const string RuleSaturdaysAt11am = "0 0 11 ? * SAT";
            const string ruleSundaysAt11Am = "0 0 11 ? * SUN";

            //Everyday at 2am
            const string RuleEverydayAt2am = "0 0 2 ? * *";

            //Everyday at 6am
            const string RuleEverydayAt6am = "0 0 6 ? * *";

            //Workdays at 9am
            const string RuleWorkdayAt9am = "0 0 9 ? * MON-FRI";

            //Workdays at 5pm
            const string ruleWorkdayAt5pm = "0 0 17 ? * MON-FRI";

            services.AddJob(GuestRoomAirconId, false, RuleEverydayAt10pm);
            services.AddJob(GuestRoomAirconId, false, RuleWorkdayAt9am);

            services.AddJob(MasterRoomAirconId, true, RuleEverydayAt10pm);
            services.AddJob(MasterRoomAirconId, false, RuleSaturdaysAt11am);
            services.AddJob(MasterRoomAirconId, false, ruleSundaysAt11Am);
            services.AddJob(MasterRoomAirconId, false, RuleWorkdayAt9am);

            services.AddJob(DiningRoomAirconId, false, RuleEverydayAt10pm);
            services.AddJob(DiningRoomAirconId, false, RuleWorkdayAt9am);

            services.AddJob(LivingRoomAirconId, false, RuleEverydayAt2am);
            services.AddJob(LivingRoomAirconId, true, RuleEverydayAt6am);
            services.AddJob(LivingRoomAirconId, false, RuleWorkdayAt9am);
            services.AddJob(LivingRoomAirconId, true, ruleWorkdayAt5pm);
            
            services.AddHostedService<QuartzHostedService>();

        }

        public static void ValidateOndoCore(this IServiceProvider services)
        {
            AzureConfigurationValidator.ValidateConfiguration(services);
        }
    }
}
