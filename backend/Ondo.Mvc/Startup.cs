using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Ondo.Api;
using Ondo.Api.Scheduler;
using Ondo.Api.Services;
using Quartz;
using Quartz.Impl;
using Quartz.Spi;

namespace Ondo.Mvc
{
    public static class ServiceCollectionJobExtensions
    {
        public static void AddJob(this IServiceCollection services, string AirConId, bool turnOn, string CronExpression)
        {
            services.AddSingleton(sp =>
            {
                return new JobSchedule(
                    airConId: AirConId,
                    turnOn: turnOn,
                    jobType: typeof(AirConJob),
                    cronExpression: CronExpression);
            });
        }
    }

    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddControllersWithViews();

            services.Configure<AzureConfiguration>(Configuration.GetSection("Azure"));
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
            const string RuleEverydayAt10pm = "0 0 22 * * * *";

            //Everyday at 10am
            const string RuleWeekendsAt11am = "0 0 11 * * 5-6 *";

            //Everyday at 2am
            const string RuleEverydayAt2am = "0 0 2 * * * *";

            //Everyday at 6am
            const string RuleEverydayAt6am = "0 0 6 * * * *";  

            //Workdays at 9am
            const string RuleWorkdayAt9am = "0 0 9 * * 0-4 *";

            //Workdays at 5pm
            const string ruleWorkdayAt5pm = "0 0 17 * * 0-4 *";

            services.AddJob(GuestRoomAirconId, false, RuleEverydayAt10pm);
            services.AddJob(GuestRoomAirconId, false, RuleWorkdayAt9am);

            services.AddJob(MasterRoomAirconId, true, RuleEverydayAt10pm);
            services.AddJob(MasterRoomAirconId, false, RuleWeekendsAt11am);
            services.AddJob(MasterRoomAirconId, false, RuleWorkdayAt9am);

            services.AddJob(DiningRoomAirconId, false, RuleEverydayAt10pm);
            services.AddJob(DiningRoomAirconId, false, RuleWorkdayAt9am);

            services.AddJob(LivingRoomAirconId, true, RuleEverydayAt2am);
            services.AddJob(LivingRoomAirconId, false, RuleEverydayAt6am);
            services.AddJob(LivingRoomAirconId, true, RuleWorkdayAt9am);
            services.AddJob(LivingRoomAirconId, false, ruleWorkdayAt5pm);


            services.AddHostedService<QuartzHostedService>();
        }

       
        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }
            else
            {
                app.UseExceptionHandler("/Home/Error");
                // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
                app.UseHsts();
            }
            app.UseHttpsRedirection();
            app.UseStaticFiles();

            app.UseRouting();

            app.UseAuthorization();

            app.UseEndpoints(endpoints =>
            {
                endpoints.MapControllerRoute(
                    name: "default",
                    pattern: "{controller=AirConOverview}/{action=Index}/{id?}");
            });
        }
    }
}
