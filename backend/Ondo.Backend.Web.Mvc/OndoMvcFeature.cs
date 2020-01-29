using System.Collections.Generic;
using System.IO;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.FileProviders;
using Microsoft.Extensions.Hosting;
using Ondo.Backend.Web.Mvc.Controllers;

namespace Ondo.Backend.Web.Mvc
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

                env.WebRootFileProvider = new PhysicalFileProvider(Path.GetFullPath(Path.Combine("..", folderName, "wwwroot")));
            }
            else
            {
                var allProviders = new List<IFileProvider>();
                // Keep the existing provider
                allProviders.Add(env.WebRootFileProvider);

                // Add a manifest provider for each relevant path
                allProviders.Add(new ManifestEmbeddedFileProvider(typeof(OndoMvcFeature).Assembly, "wwwroot"));

                // Replacing the previous WebRootFileProvider by our new composite one
                env.WebRootFileProvider = new CompositeFileProvider(allProviders);

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
