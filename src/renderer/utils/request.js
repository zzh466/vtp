
import { ipcRenderer } from 'electron';

export default function request(config){
    
    return ipcRenderer.invoke('request', config).then(res=>{
        console.log(res);
        return res;
    })
} 
