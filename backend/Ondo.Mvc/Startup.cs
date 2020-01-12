using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.HttpsPolicy;
using Microsoft.Azure.Devices.Common;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Ondo.Api;
using Ondo.Api.Services;

namespace Ondo.Mvc
{
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
            services.AddScoped<IDeviceService, DevicesService>();
            services.AddScoped<IAirConService, AirConService>();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            var logger = app.ApplicationServices.GetService<ILogger<Startup>>();

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

            var azureConfiguration = app.ApplicationServices.GetService<IOptions<AzureConfiguration>>();

            if (azureConfiguration == null || azureConfiguration.Value == null ||
                azureConfiguration.Value.IoTHubConnectionString.IsNullOrWhiteSpace())
            {
                logger.LogError("Unable to find AzureConfiguration or IoT Hub connection string.");
            }
            else
            {
                logger.LogInformation($"Application started. Configuration is {azureConfiguration.Value.IoTHubConnectionString}");
            }



        }
    }
}