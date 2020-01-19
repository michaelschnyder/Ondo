using System.IO;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.FileProviders;
using Microsoft.Extensions.Hosting;
using Ondo.Mvc.Controllers;

namespace Ondo.Mvc
{
    public static class OndoMvcFeature
    {
        public static void ConfigureOndoMvc(this IServiceCollection services) { 
           
            var assembly = typeof(HomeController).Assembly;
            services.AddControllersWithViews().AddApplicationPart(assembly);
        }

        public static void UseOndoMvc(this IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();

                var folderName = typeof(OndoMvcFeature).FullName.Substring(0, typeof(OndoMvcFeature).FullName.Length - nameof(OndoMvcFeature).Length - 1);

                env.WebRootFileProvider = new PhysicalFileProvider(Path.GetFullPath($"..\\{folderName}\\wwwroot\\"));
            }
            else
            {
                app.UseExceptionHandler("/Home/Error");
            }

            app.UseStaticFiles();

            app.UseRouting();

            app.UseEndpoints(endpoints =>
            {
                endpoints.MapControllerRoute(
                    name: "default",
                    pattern: "{controller=AirConOverview}/{action=Index}/{id?}");
            });
        }
    }
}
