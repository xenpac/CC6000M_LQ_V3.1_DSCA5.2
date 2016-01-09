

*********************************************************************
	20151203  更新至MainBranchDev_Mdl_9001_28323_289_913  
	支持AVS+  LQ3.0需要重新测试   所有工程需要重新做并测试
*********************************************************************
关键修改点：
1,新纽公版，CUSTOMER_ID == CUSTOMER_DEFAULT  （  g_customer.customer_id = CUSTOMER_DEFAULT;）
  NIT 变化后，比较是否是主频点的NIT，只有主频点的NIT版本号变化，才发送自动收台消息。
2,SDK更新至支持AVS+版本，AVS+方案和普通AVS方案按照通过ota_flash.cfg 中 
	普通的：file=btinit_spinor_ddr3_744M.img
	AVS+的：file=btinit_spinor_ddr3_624M_b0.img
  通过customer 区分
