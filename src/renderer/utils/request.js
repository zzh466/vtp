import axios from 'axios';

axios.defaults.withCredentials=true;
export default function request(config){
    const url = `http://192.168.0.108:8080/vtpmanagerapi/${config.url}`;
    return axios({...config, url}).then(res => res.data)
} 