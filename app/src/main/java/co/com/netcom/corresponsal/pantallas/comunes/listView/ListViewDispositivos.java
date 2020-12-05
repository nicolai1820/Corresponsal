package co.com.netcom.corresponsal.pantallas.comunes.listView;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import co.com.netcom.corresponsal.R;

public class ListViewDispositivos extends ArrayAdapter {


    private String[] nombreMpos;
    private String[] macMpos;
    private Activity context;

    public ListViewDispositivos(Activity context, String[] nombreMpos, String[] macMpos) {
        super(context, R.layout.list_view_item, nombreMpos);
        this.context = context;
        this.nombreMpos = nombreMpos;
        this.macMpos = macMpos;

    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        View row=convertView;
        LayoutInflater inflater = context.getLayoutInflater();
        if(convertView==null)
            row = inflater.inflate(R.layout.list_view_item, null, true);

        TextView textViewNombreMpos = (TextView) row.findViewById(R.id.textView_NombreMpos);
        TextView textViewMacMpos = (TextView) row.findViewById(R.id.textView_MacMpos);

        textViewNombreMpos.setText(nombreMpos[position]);
        textViewMacMpos.setText(macMpos[position]);
        return  row;
    }
}


