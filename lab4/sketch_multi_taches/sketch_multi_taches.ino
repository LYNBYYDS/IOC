struct Tache_s {
  unsigned int etat;  
  int config;        
}; 
struct Tache_s T1, T2;  // 2 contextes pour 2 instances de tâches ayant le même comportement

void setup_Tache(struct Tache_s *ctx, params...) {
   // Initialisation du contexte}
   ctx->etat = etat_initial;  //  reçu dans les paramètres
   ...
}

void loop_Tache(tache_t *ctx, connectors....) {   
   // test de la condition d'exécution, si absent on SORT
   if (evement_attendu_absent) return;
   // code de la tache
   ....
}
