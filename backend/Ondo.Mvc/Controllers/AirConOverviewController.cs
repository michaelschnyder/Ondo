using System.Collections.Generic;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Ondo.Backend.Core.Model;
using Ondo.Backend.Core.Services;

namespace Ondo.Mvc.Controllers
{
    public class AirConOverviewController : Controller
    {
        private IEnumerable<AirCon> _airCons = new List<AirCon>();
        private readonly IAirConService _airConsService;

        public AirConOverviewController(IAirConService airConService)
        {
            _airConsService = airConService;
        }

        // GET: AirConOverview
        public async Task<ActionResult> Index()
        {
            _airCons = await _airConsService.GetAirCons();
            return View(_airCons);
        }

        [HttpPost]
        public ActionResult Index(AirCon airCon)
        {
            _airConsService.ChangeAirConSetting(airCon);
            return View(_airCons);
        }

        // GET: AirConOverview/Edit/5
        public async Task<ActionResult> Edit(string id)
        {
            var airCon = await _airConsService.GetAirCon(id);
            return View(airCon);
        }

        // POST: AirConOverview/Edit/5
        [HttpPost]
        public ActionResult Edit(AirCon airCon)
        {
            try
            {
                _airConsService.ChangeAirConSetting(airCon);
                return RedirectToAction(nameof(Index));
            }
            catch
            {
                return View();
            }
        }
    }
}