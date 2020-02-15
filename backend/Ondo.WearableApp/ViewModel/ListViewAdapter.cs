using System.Collections.Generic;
using Android.Content;
using Android.Views;
using Android.Widget;
using GalaSoft.MvvmLight.Helpers;
using Ondo.WearableApp.Model;

namespace Ondo.WearableApp.ViewModel
{
    public class ListViewAdapter : ArrayAdapter<AirConDto>
    {
        private Context _context;
        private List<AirConDto> _items;
        private LayoutInflater _inflater;
        private DeviceViewModel _vm;

        public ListViewAdapter(Context context, List<AirConDto> items, DeviceViewModel vm) : base(context, 0, items)
        {
            _context = context;
            _items = items;
            _inflater = (LayoutInflater)_context.GetSystemService(Context.LayoutInflaterService);
            _vm = vm;
        }


        public override int Count
        {
            get
            {
                return _items.Count;
            }
        }

        public override long GetItemId(int position)
        {
            return position;
        }

        public override View GetView(int position, View view, ViewGroup parent)
        {
            AirConDto item = _items[position];
            view = _inflater.Inflate(Resource.Layout.aircons, null);

            var airconItemView = view.FindViewById<CheckBox>(Resource.Id.AirconItem);
            airconItemView.Text = item.Location;
            airconItemView.Checked = item.DevicePower;
            airconItemView.SetCommand("Click", _vm.ChangeStateOfAirCon, item);
            return view;
        }
    }
}