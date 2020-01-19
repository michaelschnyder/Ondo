using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Ondo.Api;
using Ondo.Backend.Core;
using Ondo.Backend.Core.Configuration;
using Ondo.Backend.Web.Mvc;

namespace Ondo.Backend.Web
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        // For more information on how to configure your application, visit https://go.microsoft.com/fwlink/?LinkID=398940
        public void ConfigureServices(IServiceCollection services)
        {
            services.Configure<AzureConfiguration>(Configuration.GetSection("Azure"));

            services.ConfigureOndoMvc();
            services.ConfigureOndoApi();

            services.ConfigureOndoCore();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            app.UseHttpsRedirection();
            app.UseAuthorization();

            app.UseOndoMvc(env);
            app.UseOndoApi(env);
            app.ApplicationServices.ValidateOndoCore();
        }
    }
}
