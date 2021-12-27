import {ipcMain, dialog} from'electron';
import xlsx from 'xlsx';

ipcMain.on('export-excel', (_, {title,excelData})=>{
    dialog.showSaveDialog({
        defaultPath: `../${title}-${new Date().toLocaleDateString().replace(/\//g, '')}.xlsx`
      }).then(({filePath })=>{
        if(filePath ){
          const wb  = xlsx.utils.book_new();
          const ws = xlsx.utils.json_to_sheet(excelData);
          xlsx.utils.book_append_sheet(wb,ws, title);
          xlsx.writeFile(wb, filePath) 
        }
      })
})