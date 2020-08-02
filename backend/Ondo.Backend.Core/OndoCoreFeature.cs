using System;
using Microsoft.Extensions.DependencyInjection;
using Ondo.Backend.Core.Scheduler;
using Ondo.Backend.Core.Services;
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

            //Weekend at 11am
            const string RuleWeekendAt11am = "0 0 11 ? * SAT-SUN";
            
            //Weekend at 8am
            const string RuleWeekendAt8am = "0 0 11 ? * SAT-SUN";

            //Everyday at 2am
            const string RuleEverydayAt2am = "0 0 2 ? * *";

            //Everyday at 1am
            const string RuleWorkday1am = "0 0 1 ? * MON-FRI";

            //Workdays at 9am
            const string RuleWorkdayAt9am = "0 0 9 ? * MON-FRI";

            //Workdays at 5pm
            const string RuleWorkdayAt5pm = "0 0 17 ? * MON-FRI";

            //Wordays at 7pm 
            const string RuleWorkdayAt7pm = "0 0 19 ? * MON-FRI";

            //Workdays at 12am
            const string RuleWorkdayAt12am = "0 0 12 ? * MON-FRI";

            //Workdays at 14am
            const string RuleWorkdayAt14am = "0 0 14 ? * MON-FRI";

            services.AddJob(GuestRoomAirconId, false, RuleWorkdayAt7pm);
            services.AddJob(GuestRoomAirconId, true, RuleWorkdayAt9am);

            services.AddJob(MasterRoomAirconId, true, RuleWorkdayAt9am);
            services.AddJob(MasterRoomAirconId, true, RuleEverydayAt10pm);
            services.AddJob(MasterRoomAirconId, false, RuleWeekendAt11am);
            
            services.AddJob(DiningRoomAirconId, false, RuleEverydayAt10pm);
            services.AddJob(DiningRoomAirconId, false, RuleWorkdayAt9am);

            services.AddJob(LivingRoomAirconId, false, RuleEverydayAt2am);
            services.AddJob(LivingRoomAirconId, false, RuleWorkday1am);
            services.AddJob(LivingRoomAirconId, true, RuleWeekendAt8am);
            services.AddJob(LivingRoomAirconId, false, RuleWorkdayAt9am);
            services.AddJob(LivingRoomAirconId, true, RuleWorkdayAt12am);
            services.AddJob(LivingRoomAirconId, false, RuleWorkdayAt14am);
            services.AddJob(LivingRoomAirconId, true, RuleWorkdayAt5pm);
            

            services.AddHostedService<QuartzHostedService>();

        }

        public static void ValidateOndoCore(this IServiceProvider services)
        {
            AzureConfigurationValidator.ValidateConfiguration(services);
        }
    }
}
