using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Ondo.Api.Devices;

namespace Ondo.Api
{
    public static class OndoApiFeature
    {
        public static void ConfigureOndoApi(this IServiceCollection services)
        {

            var assembly = typeof(DeviceController).Assembly;
            // services.AddControllersWithViews().AddApplicationPart(assembly);
        }

        public static void UseOndoApi(this IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            app.UseRouting();
        }
    }
}
